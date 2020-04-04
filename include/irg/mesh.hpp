#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <irg/shader.hpp>
#include <irg/mouse.hpp>

namespace irg {

  class mesh {
   public:
    using vertex_data = ::std::vector<::glm::vec3>;
    using index_data = ::std::vector<unsigned>;

   private:
    shader_program mutable mesh_shader;
    unsigned VAO, VBO, EBO;
    unsigned indice_size;

    static void normalize_data(vertex_data& vertices, index_data& indices);

   public:
    mesh(vertex_data& vertices, index_data& indices, 
         shader_program const& mesh_shader);

    static mesh from_file(char const* mesh_file, 
                          shader_program const& mesh_shader);
    static mesh from_file(char const* mesh_file, char const* vertex_shader,
                          char const* fragment_shader);

    void draw() const;

    bool is_inside(::glm::vec3 const& v);

    ~mesh();
  };

}