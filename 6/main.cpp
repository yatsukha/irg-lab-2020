#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>

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

  auto camera = ::irg::camera{};

  auto update_view_matrix = [&]{
    auto view_matrix = camera.view_matrix();
    teddy_mesh.shader->set_uniform_matrix("view", view_matrix);    
    light_mesh.shader->set_uniform_matrix("view", view_matrix);  
  };

  update_view_matrix();

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

  ::std::cout << "Enter the number of control points: ";
  ::std::size_t n; ::std::cin >> n;
  ::irg::bezier::control_points control_points(n);

  ::std::cout << "Enter each control point "
              << "(recommended values greater than 1.5): " 
              << "\n";
  for (auto& point : control_points)
    ::std::cin >> point[0] >> point[1] >> point[2];

  ::std::cout << "The camera will move along a Bezier curve defined with base "
              << "functions with an order of " << n - 1 << "."
              << "\n"; 

  ::std::cout << "Waiting 2 seconds, please switch to the main window." << "\n";

  using namespace ::std::chrono_literals;
  ::std::this_thread::sleep_for(2s);

  auto bezier_curve = ::irg::bezier::compute_from(control_points);
  float t = 0.0;
  float const delta = 0.005;

  glEnable(GL_DEPTH_TEST);
  auto counter = ::std::size_t{0};

  ::irg::window_loop(window, [&]{
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto const sin = ::std::sin(counter++ / 50.0) / 5.0;
    light_color = {0.6 + sin, 0.6 + sin, 0.6 + sin};

    light_mesh.shader->set_uniform_vec3("light_color", light_color);
    teddy_mesh.shader->set_uniform_vec3("light_color", light_color);

    if (t <= 1.0f) {
      camera.position = bezier_curve(t);
      t += delta;
    }

    update_view_matrix();

    light_mesh.draw();
    teddy_mesh.draw();

    ::irg::assert_no_error();
  });

}
