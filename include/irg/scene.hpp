#pragma once

#include <vector>
#include <functional>

#include <irg/camera.hpp>
#include <irg/generic_mesh.hpp>

#include <glm/glm.hpp>

namespace irg {
  
  class scene {
   private:
    ::std::vector<::irg::mesh_concept> objects;
    ::std::vector<bool> displayed;
    
   public:
    struct {
      float light_scaling = 0.1;
      
      float k_ambient   = 0.4;
      float k_diffuse   = 1.0;
      float k_specular  = 0.5;
      float k_roughness = 128.0;
    } configuration;
    
    ::irg::camera viewer = {};
    
    ::std::function<::glm::vec3(void)> color_supplier;
    ::std::function<::glm::vec3(void)> position_supplier;
    
    double aspect_ratio = 1.0;
    
    scene(::irg::mesh_concept&& light_source,
          ::std::function<::glm::vec3(void)> const& color_supplier,
          ::std::function<::glm::vec3(void)> const& position_supplier);
    
    ::std::size_t push_back(::irg::mesh_concept&& obj);
    ::irg::mesh_concept& operator[](::std::size_t const idx);
    
    ::irg::mesh_concept& light_source();
    
    void toggle_visibility(::std::size_t const idx);
    void draw();
    
    auto begin() {
      return objects.begin() + 1;
    }
    
    auto end() {
      return objects.end();
    }
  };
  
}