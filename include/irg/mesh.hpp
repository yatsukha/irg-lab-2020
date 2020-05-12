#pragma once

#include <vector>
#include <utility>
#include <functional>

#include <glm/glm.hpp>

#include <irg/shader.hpp>
#include <irg/mouse.hpp>
#include <irg/ownership.hpp>

namespace irg {

  class mesh {
   public:
    using vertex_data = ::std::vector<::glm::vec3>;
    using index_data = ::std::vector<::std::vector<unsigned>>;

   private:
    using vertex_normal = ::std::pair<::glm::vec3, ::glm::vec4>;
    using buffer_data  = ::std::vector<vertex_normal>;

    unsigned VAO, VBO;
    buffer_data data;

    static buffer_data normalize_data(vertex_data& vertices, 
                                      index_data& indices,
                                      bool const preserve_aspect_ratio);

   public:
    shader_program mutable shader;

    mesh(mesh const&) = delete;
    mesh& operator=(mesh const&) = delete;

    mesh(mesh&&) = default;
    mesh& operator=(mesh&&) = default;

    mesh(vertex_data& vertices, index_data& indices, 
         shader_program&& mesh_shader, bool const preserve_aspect_ratio);

    static mesh from_file(char const* mesh_file, 
                          shader_program&& mesh_shader,
                          bool const preserve_aspect_ratio = true);

    void draw() const;

    bool is_inside(::glm::vec3 const& v) const;
  };
  
  //
  // implementation
  //
  
  namespace detail {
  
    inline ::glm::vec3 normal(::glm::vec3 const& a, ::glm::vec3 const& b, 
                              ::glm::vec3 const& c) noexcept {
      return ::glm::cross(b - a, c - a);
    }
  
  }
  
  mesh::buffer_data mesh::normalize_data(vertex_data& vertices, 
                                         index_data& indices,
                                         bool const preserve_aspect_ratio) {
    for (auto& i : indices)
      for (auto& j : i)
        --j;
  
    ::glm::vec2 extremes[3]{
      {::std::numeric_limits<float>::max(), ::std::numeric_limits<float>::min()},
      {::std::numeric_limits<float>::max(), ::std::numeric_limits<float>::min()},
      {::std::numeric_limits<float>::max(), ::std::numeric_limits<float>::min()}
    };
  
    for (auto const& v : vertices)
        for (auto i = 0; i < v.length(); ++i)
          extremes[i][0] = ::std::min(extremes[i][0], v[i]),
          extremes[i][1] = ::std::max(extremes[i][1], v[i]);
  
    if (preserve_aspect_ratio) {
  
      float scaling = 0.0f;
      float lower_bound = 0.0f;
  
      for (auto i = 0; i < 3; ++i)
        if (float diff = extremes[i][1] - extremes[i][0]; scaling < diff)
          scaling = diff,
          lower_bound = extremes[i][0];
  
      scaling = 2.0 / scaling;
  
      for (auto& v : vertices)
        for (auto i = 0; i < v.length(); ++i)
          v[i] = -1.0f + (v[i] - lower_bound) * scaling;
  
    } else {
  
      ::glm::vec3 factors{
        2.0f / (extremes[0][1] - extremes[0][0]),
        2.0f / (extremes[1][1] - extremes[1][0]),
        2.0f / (extremes[2][1] - extremes[2][0])
      };
  
      for (auto& v : vertices)
        for (auto i = 0; i < v.length(); ++i)
          v[i] = -1.0f + (v[i] - extremes[i][0]) * factors[i];
    }
      
    buffer_data data;
    data.reserve(indices.size());
  
    for (auto i = 0; i < indices.size(); ++i) 
      for (auto j = 1; j < indices[i].size() - 1; ++j) {
        auto n = 
          detail::normal(
            vertices[indices[i][0]],
            vertices[indices[i][j]],
            vertices[indices[i][j + 1]]);
  
        ::glm::vec4 r{n, -::glm::dot(vertices[indices[i][0]], n)};
  
        data.emplace_back(vertices[indices[i][0]], r);
        data.emplace_back(vertices[indices[i][j]], r);
        data.emplace_back(vertices[indices[i][j + 1]], r);
      }
  
    return data;
  }
  
  mesh::mesh(vertex_data& vertices, index_data& indices, 
             shader_program&& mesh_shader, 
             bool const preserve_aspect_ratio)
    : shader(mesh_shader)
    , data(normalize_data(vertices, indices, preserve_aspect_ratio))
  {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
  
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
      GL_ARRAY_BUFFER, 
      sizeof(buffer_data::value_type) * data.size(),
      data.data(), 
      GL_STATIC_DRAW
    );
  
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 
      sizeof(buffer_data::value_type), nullptr
    );
  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, 
      sizeof(buffer_data::value_type), 
      reinterpret_cast<void*>(sizeof(buffer_data::value_type::first_type))
    );
  }
  
  bool mesh::is_inside(::glm::vec3 const& v) const {
    for (auto const& [_, r] : data)
      if (::glm::dot(r, {v, 1.0}) > 0)
        return false;
    return true;
  }
  
  void mesh::draw() const {
    shader.activate();
    glBindVertexArray(VAO);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, data.size());
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  
  mesh mesh::from_file(char const* mesh_file, shader_program&& mesh_shader,
                       bool const preserve_aspect_ratio) {
    vertex_data vertices;
    index_data indices;
  
    ::std::ifstream in(mesh_file);
  
    ::std::string c;
    bool expanded = false;
    
    while (in >> c)
      if (c == "v") {
        vertices.push_back({});
        auto& v = vertices.back();
        in >> v[0] >> v[1] >> v[2];
      } else if (c == "f") {
        indices.push_back({});
        ::std::string line;
        ::std::getline(in, line);
        ::std::istringstream ss(line);
        ::std::string token;
        while (ss >> token)
          if (::std::isdigit(token[0])) {
            indices.back().push_back(::std::stoi(token));
            ss.ignore(::std::numeric_limits<::std::streamsize>::max(), ' ');
          }
      } else {
        in.ignore(::std::numeric_limits<::std::streamsize>::max(), '\n');
      }
  
    return { 
      vertices, indices, 
      ::std::forward<shader_program>(mesh_shader), preserve_aspect_ratio };
  }

}