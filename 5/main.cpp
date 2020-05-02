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

  auto fur_texture = ::irg::texture("textures/bear_fur.jpg");
  fur_texture.use();

  auto teddy_mesh = ::irg::mesh::from_file(
    "objects/teddy.obj",
    {
      {"shaders/vertex.glsl", GL_VERTEX_SHADER},
      {"shaders/fragment.glsl", GL_FRAGMENT_SHADER}
    }
  );

  teddy_mesh.shader->set_uniform_int("texture_present", 1);

  teddy_mesh.shader->set_uniform_vec3("light_position", light_position);
  teddy_mesh.shader->set_uniform_vec3("light_color", light_color);

  auto cam_position  = ::glm::vec3{0.0, 0.0, 10.0};
  auto cam_target    = ::glm::vec3{0.0, 0.0, 0.0};

  auto update_view_matrix = [&]{
    auto view_matrix =
      ::glm::lookAt(
          cam_position, 
          cam_target,
          ::glm::vec3{0.0, 1.0, 0.0}
      );
    teddy_mesh.shader->set_uniform_matrix("view", view_matrix);    
    light_mesh.shader->set_uniform_matrix("view", view_matrix);  
  };

  update_view_matrix();

  ::irg::k_events.add_listener(
  [&](auto const key, bool const released) mutable {
    if (released)
      return ::irg::ob::action::remain;

    auto direction = ::glm::vec4{cam_position - cam_target, 1.0};

    auto const angle = ::glm::radians(10.0f);
    auto const scale = 0.5f;

    auto rotate_around = [&](auto&& object, ::glm::vec3 const& axis) {
      object += 
        ::glm::vec3(::glm::angleAxis(angle, axis) * direction - direction);
    };

    if (key == GLFW_KEY_A)
      rotate_around(cam_position, {0.0, 1.0, 0.0});
    else if (key == GLFW_KEY_D)
      rotate_around(cam_position, {0.0, -1.0, 0.0});
    else if (key == GLFW_KEY_W)
      rotate_around(cam_position, {-1.0, 0.0, 0.0});
    else if (key == GLFW_KEY_S)
      rotate_around(cam_position, {1.0, 0.0, 0.0});
    else if (key == GLFW_KEY_I)
      cam_position -= scale * ::glm::normalize(::glm::vec3(direction));
    else if (key == GLFW_KEY_O)
      cam_position += scale * ::glm::normalize(::glm::vec3(direction));
    else if (key == GLFW_KEY_LEFT)
      rotate_around(cam_target, {0.0, -1.0, 0.0});

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
