#include <irg/mesh.hpp>
#include <irg/mouse.hpp>

#include <fstream>
#include <limits>

namespace irg {

  namespace detail {

    inline ::glm::vec3 normal(::glm::vec3 const& a, ::glm::vec3 const& b, 
                              ::glm::vec3 const& c) noexcept {
      return ::glm::cross(b - a, c - a);
    }

  }

  mesh::buffer_data mesh::normalize_data(vertex_data& vertices, 
                                         index_data& indices) {
    for (auto& i : indices)
      --i;

    ::glm::vec2 extremes[3]{
      {::std::numeric_limits<float>::max(), ::std::numeric_limits<float>::min()},
      {::std::numeric_limits<float>::max(), ::std::numeric_limits<float>::min()},
      {::std::numeric_limits<float>::max(), ::std::numeric_limits<float>::min()}
    };

    for (auto const& v : vertices)
      for (auto i = 0; i < v.length(); ++i)
        extremes[i][0] = ::std::min(extremes[i][0], v[i]),
        extremes[i][1] = ::std::max(extremes[i][1], v[i]);

    ::glm::vec3 factors{
      2.0f / (extremes[0][1] - extremes[0][0]),
      2.0f / (extremes[1][1] - extremes[1][0]),
      2.0f / (extremes[2][1] - extremes[2][0])
    };

    for (auto& v : vertices)
      for (auto i = 0; i < v.length(); ++i)
        v[i] = -1.0f + (v[i] - extremes[i][0]) * factors[i];
    
    buffer_data data;
    data.reserve(indices.size());

    for (auto i = 0; i < indices.size(); i += 3) {
      auto normal = 
      detail::normal(
        vertices[indices[i]], 
        vertices[indices[i + 1]], 
        vertices[indices[i + 2]]);

      for (auto j = 0; j < 3; ++j)
        data.emplace_back(
          vertices[indices[i + j]],
          ::glm::vec4{
            normal.x, normal.y, normal.z, 
            -::glm::dot(vertices[indices[i + j]], normal)
          }
        );
    }

    return data;
  }

  mesh::mesh(vertex_data& vertices, index_data& indices, 
             shader_program const& mesh_shader)
    : mesh_shader(mesh_shader), indice_size(indices.size())
  {
    auto data = normalize_data(vertices, indices);

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

  void mesh::draw() const {
    mesh_shader.activate();
    glBindVertexArray(VAO);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, indice_size);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  mesh::~mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }

  mesh mesh::from_file(char const* mesh_file, shader_program const& mesh_shader) {
    vertex_data vertices;
    index_data indices;

    ::std::ifstream in(mesh_file);

    char c;
    
    while (in >> c)
      switch (c) {
        case '#':
          in.ignore(::std::numeric_limits<::std::streamsize>::max(), '\n');
          break;
        case 'v': {
          vertices.push_back({});
          auto& v = vertices.back();
          in >> v[0] >> v[1] >> v[2];
          break;
        }
        case 'f':
          indices.push_back({});
          in >> indices.back();
          indices.push_back({});
          in >> indices.back();
          indices.push_back({});
          in >> indices.back();;
          break;
        case 'g':
          break;
      }

    return { vertices, indices, mesh_shader };
  }

  mesh mesh::from_file(char const* mesh_file, char const* vertex_shader,
                       char const* fragment_shader) {
    return mesh::from_file(mesh_file, shader_program(
      shader(vertex_shader, GL_VERTEX_SHADER),
      shader(fragment_shader, GL_FRAGMENT_SHADER)
    ));
  }

}