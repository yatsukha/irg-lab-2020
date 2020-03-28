#pragma once

#include <string>
#include <fstream>
#include <streambuf>
#include <cstring>
#include <array>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    void set_uniform_color(char const* uniform_name, color const& c) {
      glUniform3f(glGetUniformLocation(*id, uniform_name), c.r, c.g, c.b);
    }
  };

}