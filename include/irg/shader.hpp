#pragma once

#include <string>
#include <fstream>
#include <streambuf>
#include <cstring>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irg/common.hpp>

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
   public:
    unsigned id;

    shader_program(shader const& vertex, shader const& fragment)
      : id(glCreateProgram())
    {
      glAttachShader(id, vertex.id);
      glAttachShader(id, fragment.id);

      glLinkProgram(id);

      int success;
      ::std::array<char, 512> log;

      glGetProgramiv(id, GL_LINK_STATUS, &success);
      if (!success)
        glGetProgramInfoLog(id, log.max_size(), nullptr, log.data()),
        ::irg::terminate(log.data());
    }

    ~shader_program() {
      glDeleteProgram(id);
    }

    void activate() noexcept {
      glUseProgram(id);
    }
  };

}