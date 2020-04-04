#include <irg/texture.hpp>

#include <irg/common.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

namespace irg {

  texture::texture(char const* file) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, nc;
    unsigned char* data = stbi_load(file, &w, &h, &nc, 0);
    on_scope_exit data_guard{[&data]{
      stbi_image_free(reinterpret_cast<void*>(data));
    }};

    if (!data)
      terminate("Error while loading texture.");

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
  }

  void texture::use() {
    glBindTexture(GL_TEXTURE_2D, id);
  }

  texture::~texture() {
    glDeleteTextures(1, &id);
  }

}