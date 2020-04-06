#pragma once

#include <string>
#include <fstream>
#include <streambuf>
#include <cstring>
#include <array>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <irg/common.hpp>
#include <irg/primitive.hpp>

namespace irg {

  class shader {
   public:
    unsigned type;
    unsigned id;

    shader(char const* file, int const type)
      : type(type), id(glCreateShader(type))
    {
      ::std::ifstream f(file);
      if (!f.is_open())
        ::std::cerr << "Error while opening file: ",
        ::irg::terminate(file);

      ::std::string source(
        (::std::istreambuf_iterator<char>(f)),
        ::std::istreambuf_iterator<char>()
      );

      const char* chars = source.c_str();

      glShaderSource(id, 1, &chars, nullptr);
      glCompileShader(id);

      int success;
      ::std::array<char, 512> log;

      glGetShaderiv(id, GL_COMPILE_STATUS, &success);
      if (!success)
        glGetShaderInfoLog(id, log.max_size(), nullptr, log.data()),
        ::std::cerr << file << ": " << "\n",
        ::irg::terminate(log.data());
    }

    ~shader() {
      glDeleteShader(id);
    }
  };

  class shader_program {
    ::std::shared_ptr<unsigned> id;

   public:
    shader_program(shader const& vertex, shader const& fragment) 
      : id(
          ::std::shared_ptr<unsigned>(
            new unsigned(glCreateProgram()), 
            [](unsigned* ptr) {
              delete ptr;
            }
          )
        )
    {
      glAttachShader(*id, vertex.id);
      glAttachShader(*id, fragment.id);

      glLinkProgram(*id);

      int success;
      ::std::array<char, 512> log;

      glGetProgramiv(*id, GL_LINK_STATUS, &success);
      if (!success)
        glGetProgramInfoLog(*id, log.max_size(), nullptr, log.data()),
        ::irg::terminate(log.data());
    }

    void activate() noexcept {
      glUseProgram(*id);
    }

    void set_uniform_int(char const* uniform_name, int const i) {
      activate();
      glUniform1i(glGetUniformLocation(*id, uniform_name), i);
    }

    void set_uniform_color(char const* uniform_name, color const& c) {
      activate();
      glUniform3f(glGetUniformLocation(*id, uniform_name), c.r, c.g, c.b);
    }

    void set_uniform_vec3(char const* uniform_name, ::glm::vec3 const &v) {
      activate();
      glUniform3fv(
        glGetUniformLocation(*id, uniform_name), 
        1, ::glm::value_ptr(v)
      );
    }

    ::glm::mat4 get_uniform_matrix(char const* uniform_name) {
      activate();
      ::std::unique_ptr<float[]> mat(new float[16]);
      glGetUniformfv(*id, glGetUniformLocation(*id, uniform_name), mat.get());

      return ::glm::make_mat4(mat.get());
    }

    void set_uniform_matrix(char const* uniform_name, ::glm::mat4 const& m) {
      activate();
      glUniformMatrix4fv(
        glGetUniformLocation(*id, uniform_name), 
        1, GL_FALSE, ::glm::value_ptr(m)
      );
    }

    void transform_matrix(char const* uniform_name, ::glm::mat4 const& m) {
      activate();
      set_uniform_matrix(uniform_name, m * get_uniform_matrix(uniform_name));
    }
    
  };

}