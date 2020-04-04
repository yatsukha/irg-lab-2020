#include <irg/mesh.hpp>
#include <irg/mouse.hpp>

#include <fstream>
#include <limits>

namespace irg {

  void mesh::normalize_data(vertex_data& vertices, index_data& indices) {
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
      1.0f / (extremes[0][1] - extremes[0][0]),
      1.0f / (extremes[1][1] - extremes[1][0]),
      1.0f / (extremes[2][1] - extremes[2][0])
    };

    for (auto& v : vertices)
      for (auto i = 0; i < v.length(); ++i)
        v[i] = -0.5f + (v[i] - extremes[i][0]) * factors[i];
  }

  mesh::mesh(vertex_data& vertices, index_data& indices, 
             shader_program const& mesh_shader)
    : mesh_shader(mesh_shader), indice_size(indices.size())
  {
    normalize_data(vertices, indices);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
      GL_ARRAY_BUFFER, 
      sizeof(vertex_data::value_type) * vertices.size(),
      vertices.data(), 
      GL_STATIC_DRAW
    );

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      sizeof(index_data::value_type) * indices.size(),
      indices.data(),
      GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 
      sizeof(::std::remove_reference_t<decltype(vertices)>::value_type), 
      nullptr
    );
  }

  void mesh::draw() const {
    mesh_shader.activate();
    glBindVertexArray(VAO);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, indice_size, GL_UNSIGNED_INT, nullptr);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  mesh::~mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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