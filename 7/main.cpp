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

#include <GLFW/glfw3.h>

int main() {
  auto  guard  = ::irg::init();
  auto* window = ::irg::create_window(800, 800);

  ::irg::bind_events(window);

  auto light_mesh = ::irg::mesh::from_file(
    "objects/kocka.obj",
    {
      {"shaders/light_vertex.glsl", GL_VERTEX_SHADER},
      {"shaders/light_fragment.glsl", GL_FRAGMENT_SHADER}
    }
  );

  auto light_position = ::glm::vec3{0.8, 0.8, 0.8};
  auto light_color    = ::glm::vec3{0.8, 0.8, 0.8};
  auto view_scaling   = ::glm::vec3(0.5);

  light_mesh.shader->set_uniform_matrix(
    "model",
    ::glm::scale(
      ::glm::translate(::glm::mat4(1.0), light_position),
      ::glm::vec3(0.1)
    )
  );

  light_mesh.shader->set_uniform_matrix(
    "view", ::glm::scale(::glm::mat4(1.0), view_scaling));

  light_mesh.shader->set_uniform_vec3("light_color", light_color);

  auto teddy_mesh = ::irg::mesh::from_file(
    "objects/teddy.obj",
    {
      {"shaders/vertex.glsl", GL_VERTEX_SHADER},
      {"shaders/fragment.glsl", GL_FRAGMENT_SHADER}
    }
  );

  teddy_mesh.shader->set_uniform_vec3("light_position", light_position);
  teddy_mesh.shader->set_uniform_vec3("light_color", light_color);
  
  // auto fur_texture = ::irg::texture("textures/bear_fur.jpg");
  // fur_texture.use();
  
  // teddy_mesh.shader->set_uniform_int("texture_present", 1);

  auto camera = ::irg::camera{};

  auto update_view_matrix = [&]{
    auto view_matrix = camera.view_matrix();
    teddy_mesh.shader->set_uniform_matrix("view", view_matrix);    
    light_mesh.shader->set_uniform_matrix("view", view_matrix);  
  };

  update_view_matrix();

  ::std::cout << "Use WASD to move camera, arrow keys to move camera target, "
              << "IO to move camera in and out, JK to move camera target "
              << "in and out. Warning: the last can be quite confusing."
              << ::std::endl;
  
  bool reversed = false;
  
  ::std::cout << "Use 1234 keys to control ambient, diffuse, specular and "
              << "roughness components of Phong's light model. "
              << "Use left shift + above keys for inverse operations."
              << ::std::endl;
  
  ::std::cout << "Note that the light pulsates, so take time to observe."
              << ::std::endl;

  ::irg::k_events.add_listener(
  [&](auto const key, bool const released) mutable {
    auto adjust = [&](auto&& n) { return released ? -n : n; };
      
    // cam movement
    if (key == GLFW_KEY_A)
      camera.position_mask[1] += adjust(1.0);
    else if (key == GLFW_KEY_D)
      camera.position_mask[1] += adjust(-1.0);
    else if (key == GLFW_KEY_W)
      camera.position_mask[0] += adjust(1.0);
    else if (key == GLFW_KEY_S)
      camera.position_mask[0] += adjust(-1.0);
    else if (key == GLFW_KEY_I)
      camera.zoom_mask[0] += adjust(1.0);
    else if (key == GLFW_KEY_O)
      camera.zoom_mask[0] += adjust(-1.0);
    else if (key == GLFW_KEY_LEFT)
      camera.target_mask[1] += adjust(1.0);
    else if (key == GLFW_KEY_RIGHT)
      camera.target_mask[1] += adjust(-1.0);
    else if (key == GLFW_KEY_UP)
      camera.target_mask[0] += adjust(1.0);
    else if (key == GLFW_KEY_DOWN)
      camera.target_mask[0] += adjust(-1.0);
    else if (key == GLFW_KEY_J)
      camera.zoom_mask[1] += adjust(1.0);
    else if (key == GLFW_KEY_K)
      camera.zoom_mask[1] += adjust(-1.0);
    // Phong light model modification
    else if (key == GLFW_KEY_LEFT_SHIFT)
      reversed = !released;
    else if (key == GLFW_KEY_1 && !released)
      teddy_mesh.shader->transform_uniform_float("k_ambient", [reversed](auto&& f) {
        auto ret = f + (reversed ? -0.1f : 0.1f);
        ::std::cout << "Ambient changed to: " << ret << ::std::endl;
        return ret;
      });
    else if (key == GLFW_KEY_2 && !released)
      teddy_mesh.shader->transform_uniform_float("k_diffuse", [reversed](auto&& f) {
        auto ret = f + (reversed ? -0.1f : 0.1f);
        ::std::cout << "Diffuse changed to: " << ret << ::std::endl;
        return ret;
      });
    else if (key == GLFW_KEY_3 && !released)
      teddy_mesh.shader->transform_uniform_float("k_specular", [reversed](auto&& f) {
        auto ret = f + (reversed ? -0.1f : 0.1f);
        ::std::cout << "Specular changed to: " << ret << ::std::endl;
        return ret;
      });
    else if (key == GLFW_KEY_4 && !released)
      teddy_mesh.shader->transform_uniform_float("k_roughness", [reversed](auto&& f) {
        auto ret = f * (reversed ? 0.5f : 2.0f);
        ::std::cout << "Roughness changed to: " << ret << ::std::endl;
        return ret;
      });

    return ::irg::ob::action::remain;
  });

  auto update_projection_matrix = [&](auto&& ratio) {
    auto proj_matrix = 
      ::glm::perspective(::glm::radians(45.0), ratio, 0.1, 100.0);

    teddy_mesh.shader->set_uniform_matrix("projection", proj_matrix);
    light_mesh.shader->set_uniform_matrix("projection", proj_matrix);
  };

  update_projection_matrix(1.0);
  
  ::irg::w_events.add_listener([&](auto const w, auto const h) {
    update_projection_matrix(static_cast<double>(w) / h);
    return ::irg::ob::action::remain;
  });

  glEnable(GL_DEPTH_TEST);
  auto counter = ::std::size_t{0};

  ::irg::window_loop(window, [&]{
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto const sin = ::std::sin(counter++ / 50.0) / 5.0;
    light_color = {0.6 + sin, 0.6 + sin, 0.6 + sin};

    light_mesh.shader->set_uniform_vec3("light_color", light_color);
    teddy_mesh.shader->set_uniform_vec3("light_color", light_color);

    update_view_matrix();    

    light_mesh.draw();
    teddy_mesh.draw();

    ::irg::assert_no_error();
  });

}
