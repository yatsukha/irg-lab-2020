#include <algorithm>
#include <vector>
#include <memory>
#include <unordered_set>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include <irg/common.hpp>
#include <irg/shader.hpp>
#include <irg/mouse.hpp>
#include <irg/shape.hpp>
#include <irg/mesh.hpp>
#include <irg/texture.hpp>

#define GUARD __FILE__##guard##__LINE__

int main(int const argc, char const* const* argv) {
  if (argc != 2)
    ::irg::terminate("Provide object file as a command line argument.");

  auto  GUARD  = ::irg::init();
  auto* window = ::irg::create_window(800, 800);

  ::glfwSetMouseButtonCallback(window, ::irg::simple_mouse_events::click_callback);
  ::glfwSetCursorPosCallback(window, ::irg::simple_mouse_events::move_callback);

  ::glfwSetKeyCallback(window, ::irg::keyboard_events::callback);

  ::glm::vec3 light_position{0.5f, 0.5f, 0.8f};

  // loading light source

  ::irg::shader_program light_shader{
    ::irg::shader("./4/shaders/light_vertex.glsl", GL_VERTEX_SHADER),
    ::irg::shader("./4/shaders/light_fragment.glsl", GL_FRAGMENT_SHADER)
  };

  light_shader.activate();

  // scale and move the light source
  light_shader.set_uniform_matrix(
    "model",
    ::glm::scale(
      ::glm::translate(::glm::mat4(1.0f), light_position), 
      ::glm::vec3(0.2f))
  );

  ::irg::mesh cube_mesh =
    ::irg::mesh::from_file("./4/objects/kocka.obj", light_shader);

  // loading given mesh

  ::irg::shader_program mesh_shader{
    ::irg::shader("./4/shaders/vertex.glsl", GL_VERTEX_SHADER),
    ::irg::shader("./4/shaders/fragment.glsl", GL_FRAGMENT_SHADER)
  };

  mesh_shader.activate();
  
  mesh_shader.set_uniform_color("light_color", {1.0f, 1.0f, 1.0f});
  mesh_shader.set_uniform_vec3("light_position", light_position);

  // scale the model down to half, so it fits nicely in the screen
  mesh_shader.transform_matrix(
    "model",
    ::glm::scale(::glm::mat4(1.0f), ::glm::vec3(0.5f))
  );

  ::irg::mesh mesh = 
    ::irg::mesh::from_file(argv[1], mesh_shader);
  ::irg::texture tex{"./4/textures/marble_texture.jpg"};

  // object rotation

  auto s = ::std::sin(-M_PI_2);
  auto c = ::std::cos(-M_PI_2);

  ::glm::mat3 const r90{{c, s, 0}, {-s, c, 0}, {0, 0, 1}};

  ::glm::vec3 movement;
  bool tracking = false;
  auto* cursor = ::glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
  ::irg::on_scope_exit cursor_guard{
    [&cursor]{
      ::glfwDestroyCursor(cursor);
    }
  };

  bool object_rotation = false;

  ::irg::k_events.add_listener(
    [&object_rotation](auto const key) {
      if (key == GLFW_KEY_LEFT_CONTROL)
        object_rotation = !object_rotation;
      return ::irg::ob::action::remain;
    }
  );

  ::irg::sm_events.add_listener({
    [&](auto const& point) {
      if (tracking = !tracking)
        ::glfwSetCursor(window, cursor),
        movement = {point.x, point.y, 0};
      else
        ::glfwSetCursor(window, nullptr);
      return ::irg::ob::action::remain;
    },
    [&](auto const& point) {
      if (tracking) {
        ::glm::vec3 updated = {point.x, point.y, 0};
        ::glm::vec3 diff = ::glm::normalize(r90 * (updated - movement));

        auto mat = ::glm::toMat4(::glm::angleAxis(::glm::radians(1.2f), diff));

        movement = updated;

        mesh_shader.activate();

        if (object_rotation) {
          mesh_shader.transform_matrix("model", mat);
        } else {
          mesh_shader.transform_matrix("view", mat);

          light_shader.activate();
          light_shader.transform_matrix("view", mat);
        }
      }

      return ::irg::ob::action::remain;
    }
  });

  glEnable(GL_DEPTH_TEST);
  ::glm::vec3 counters(0.0f);

  ::irg::window_loop(window, [&] {
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    counters[0] += 0.01f;
    counters[1] += 0.02f;
    counters[2] += 0.035f;
    
    ::irg::color c{
      0.5f + ::std::sin(counters[0]) / 2.0f,
      0.5f + ::std::sin(counters[1]) / 2.0f,
      0.5f + ::std::sin(counters[2]) / 2.0f
    };

    mesh_shader.activate();
    mesh_shader.set_uniform_color("light_color", c);

    light_shader.activate();
    light_shader.set_uniform_color("light_color", c);

    tex.use();
    mesh.draw();
    cube_mesh.draw();

    ::irg::assert_no_error();
  });
}