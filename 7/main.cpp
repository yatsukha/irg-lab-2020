#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <irg/common.hpp>
#include <irg/mesh.hpp>
#include <irg/shader.hpp>
#include <irg/keyboard.hpp>
#include <irg/window.hpp>
#include <irg/texture.hpp>
#include <irg/camera.hpp>
#include <irg/scene.hpp>

#include <GLFW/glfw3.h>

int main() {
  auto  guard  = ::irg::init();
  auto* window = ::irg::create_window(800, 800);

  ::irg::bind_events(window);
  
  ::irg::scene scene{
    ::irg::mesh::from_file(
      "objects/kocka.obj",
      {{"shaders/light_vertex.glsl", GL_VERTEX_SHADER},
       {"shaders/light_fragment.glsl", GL_FRAGMENT_SHADER}}),
    []{
      ::std::size_t static counter = 0;
      auto const sin = ::std::sin(counter++ / 50.0) / 5.0;
      return ::glm::vec3{0.6 + sin, 0.6 + sin, 0.6 + sin};
    },
    []{
      ::std::size_t static counter = 0;
      auto const val = counter++ / 50.0;
      
      return ::glm::vec3{
        2 * ::std::sin(val),
        2 * ::std::sin(val / 10.0),
        2 * ::std::cos(val)
      };
    }
  };
  
  auto phong_teddy = scene.push_back(::irg::generic_mesh<
      ::irg::vertex_policies::simple_vertex_policy
    >::from_file(
      "objects/teddy.obj",
      {
        {"shaders/vertex.glsl", GL_VERTEX_SHADER},
        {"shaders/fragment.glsl", GL_FRAGMENT_SHADER}
      }
  ));
  
  auto gouraud_teddy = scene.push_back(::irg::generic_mesh<
      ::irg::vertex_policies::gouraud_vertex_policy
    >::from_file(
      "objects/teddy.obj",
      {
        {"shaders/vertex.glsl", GL_VERTEX_SHADER},
        {"shaders/fragment.glsl", GL_FRAGMENT_SHADER}
      }
  ));
  
  scene[gouraud_teddy].shader()->set_uniform_int("gouraud_shading", 1);
  scene.toggle_visibility(gouraud_teddy);

  ::std::cout << "Use WASD to move camera, arrow keys to move camera target, "
              << "IO to move camera in and out, JK to move camera target "
              << "in and out. Warning: the last can be quite confusing."
              << ::std::endl;
  
  ::std::cout << "Use 1234 keys to control ambient, diffuse, specular and "
              << "roughness components of Phong's light model. "
              << "Use left shift + above keys for inverse operations."
              << ::std::endl;
  
  ::std::cout << "Use G to toggle gourad shading." << ::std::endl;
  
  ::std::cout << "Note that the light pulsates, so take time to observe."
              << ::std::endl;
  
  ::irg::k_events.add_listener(::irg::standard_camera_controler(scene.viewer));

  ::irg::k_events.add_listener(
  [&](auto const key, bool const released) mutable {
    auto static reversed = false;
    
    auto& [_, k_a, k_d, k_s, k_r] = scene.configuration;
    
    // Phong light model modification
    if (key == GLFW_KEY_LEFT_SHIFT)
      reversed = !released;
    else if (key == GLFW_KEY_1 && !released)
      k_a += reversed ? -0.1f : 0.1f,
      ::std::cout << "Ambient changed to: " << k_a << ::std::endl;
    else if (key == GLFW_KEY_2 && !released)
      k_d += reversed ? -0.1f : 0.1f,
      ::std::cout << "Diffuse changed to: " << k_d << ::std::endl;
    else if (key == GLFW_KEY_3 && !released)
      k_s += reversed ? -0.1f : 0.1f,
      ::std::cout << "Specular changed to: " << k_s << ::std::endl;
    else if (key == GLFW_KEY_4 && !released)
      k_r *= reversed ? 0.5f : 2.0f,
      ::std::cout << "Roughness changed to: " << k_r << ::std::endl;
    else if (key == GLFW_KEY_G && !released)
      scene.toggle_visibility(phong_teddy),
      scene.toggle_visibility(gouraud_teddy);

    return ::irg::ob::action::remain;
  });

  glEnable(GL_DEPTH_TEST);

  ::irg::window_loop(window, [&]{
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene.draw();
    ::irg::assert_no_error();
  });

}
