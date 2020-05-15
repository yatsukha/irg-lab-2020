#include <irg/scene.hpp>

#include <cassert>

namespace irg {
  
  scene::scene(::irg::mesh_concept&& light_source,
               ::std::function<::glm::vec3(void)> const& color_supplier,
               ::std::function<::glm::vec3(void)> const& position_supplier)
    : color_supplier(color_supplier)
    , position_supplier(position_supplier)
  {
    objects.emplace_back(::std::forward<::irg::mesh_concept>(light_source));
    displayed.push_back(true);
  }
  
  ::std::size_t scene::push_back(::irg::mesh_concept&& obj) {
    objects.emplace_back(::std::forward<::irg::mesh_concept>(obj));
    displayed.push_back(true);
    
    return objects.size() - 1;
  }
  
  ::irg::mesh_concept& scene::operator[](::std::size_t const idx) {
    return objects[idx];
  }
  
  ::irg::mesh_concept& scene::light_source() {
    return objects[0];
  }
  
  void scene::toggle_visibility(::std::size_t const idx) {
    displayed[idx] = !displayed[idx];
  }
  
  void scene::draw() {
    assert(color_supplier);
    assert(objects.size() == displayed.size());
    
    
    auto light_color    = color_supplier();
    auto light_position = position_supplier();
        
    auto view_matrix = viewer.view_matrix();
    auto proj_matrix =
      ::glm::perspective(::glm::radians(45.0), aspect_ratio, 0.1, 100.0);
    
    if (displayed[0]) {
      objects[0].shader()->set_uniform_matrix(
        "model",
        ::glm::scale(::glm::translate(::glm::mat4(1.0), light_position),
                     ::glm::vec3(configuration.light_scaling))
      );
    }
    
    for (::std::size_t i = 0; i < objects.size(); ++i) {
      if (displayed[i]) {
        auto s = objects[i].shader();
        
        s->set_uniform_matrix("view", view_matrix);
        s->set_uniform_matrix("projection", proj_matrix);
        
        s->set_uniform_vec3("light_color", light_color);
        s->set_uniform_vec3("light_position", light_position);
        
        if (i) {
          s->set_uniform_float("k_ambient",   configuration.k_ambient);
          s->set_uniform_float("k_diffuse",   configuration.k_diffuse);
          s->set_uniform_float("k_specular",  configuration.k_specular);
          s->set_uniform_float("k_roughness", configuration.k_roughness);
        }
        
        objects[i].draw();
      }
    }
  }
  
}