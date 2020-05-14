#pragma once

#include <vector>
#include <utility>
#include <sstream>
#include <fstream>
#include <string>
#include <cassert>

#include <glm/glm.hpp>

#include <irg/shader.hpp>
#include <irg/ownership.hpp>

namespace irg {
  
  template<typename VertexPolicy>
  class generic_mesh {
   public:
    using vertex_data = ::std::vector<::glm::vec3>;
    using index_data = ::std::vector<::std::vector<unsigned>>;
    
    using vertex_pair =
      ::std::pair<::glm::vec3, typename VertexPolicy::attached_info>;
    using buffer_data = ::std::vector<vertex_pair>;
    
   private:
    unsigned VAO, VBO;
    buffer_data data;
    
    static buffer_data normalize_data(vertex_data& vertices,
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
      
      return VertexPolicy::compute_buffer_data(vertices, indices);
    }
    
   public:
    shader_program mutable shader;
    
    generic_mesh(vertex_data& vertices, index_data& indices,
                 shader_program&& shader,
                 bool const preserve_aspect_ratio)
      : shader(::std::forward<shader_program>(shader))
      , data(normalize_data(vertices, indices, preserve_aspect_ratio))
    {
      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      
      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(typename buffer_data::value_type) * data.size(),
        data.data(), 
        GL_STATIC_DRAW
      );
      
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 
        sizeof(typename buffer_data::value_type), nullptr
      );
      
      VertexPolicy::attrib_init(data);
    }
    
    generic_mesh(generic_mesh const&) = delete;
    generic_mesh& operator=(generic_mesh const&) = delete;
    
    generic_mesh(generic_mesh&&) = default;
    generic_mesh& operator=(generic_mesh&&) = default;
    
    static generic_mesh from_file(char const* mesh_file,
                                  shader_program&& shader,
                                  bool const preserve_aspect_ratio = true) {
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
        ::std::forward<shader_program>(shader), preserve_aspect_ratio };
    }
    
    void draw() const {
      shader.activate();
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, data.size());
    }
    
    bool is_inside(::glm::vec3 const& v) const {
      return VertexPolicy::is_inside(v, data);
    }
  };
  
  namespace vertex_policies {
    
    struct simple_vertex_policy {
      using attached_info = ::glm::vec4;
      using mesh_type = generic_mesh<simple_vertex_policy>;
      
      static mesh_type::buffer_data compute_buffer_data(
          mesh_type::vertex_data const& vertices,
          mesh_type::index_data const& indices) {
        auto normal = [](auto&& a, auto&& b, auto&& c) {
          return ::glm::cross(b - a, c - a);
        };
        
        mesh_type::buffer_data data;
        
        for (auto i = 0; i < indices.size(); ++i) 
          for (auto j = 1; j < indices[i].size() - 1; ++j) {
            auto n = normal(
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
      
      static void attrib_init(mesh_type::buffer_data const& data) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
          1, 4, GL_FLOAT, GL_FALSE,
          sizeof(mesh_type::buffer_data::value_type),
          reinterpret_cast<void*>(sizeof(
            mesh_type::buffer_data::value_type::first_type
          ))
        );
      }
      
      static bool is_inside(::glm::vec3 const& v,
                     mesh_type::buffer_data const& data) {
        for (auto const& [_, r] : data)
          if (::glm::dot(r, {v, 1.0}) > 0)
            return false;
        return true;
      }
    };
   
  }
  
}