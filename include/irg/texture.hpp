#pragma once

#include <irg/ownership.hpp>

namespace irg {

  class texture {
    shared_ownership<unsigned> id;

   public:
    texture() {};
    texture(char const* path);
    void use();
  };

}