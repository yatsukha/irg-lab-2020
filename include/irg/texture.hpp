#pragma once

namespace irg {

  class texture {
    unsigned id;

   public:
    texture(char const* path);
    
    void use();

    ~texture();
  };

}