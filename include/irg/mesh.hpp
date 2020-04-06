#pragma once

#include <vector>
#include <utility>

#include <glm/glm.hpp>

#include <irg/shader.hpp>
#include <irg/mouse.hpp>

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

    bool is_inside(::glm::vec3 const& v);

    ~mesh();
  };

}