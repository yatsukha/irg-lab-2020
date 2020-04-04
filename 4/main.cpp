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

  ::irg::shader_program mesh_shader{
    ::irg::shader("./4/vertex.glsl", GL_VERTEX_SHADER),
    ::irg::shader("./4/fragment.glsl", GL_FRAGMENT_SHADER)
  };

  mesh_shader.activate();

  ::irg::mesh mesh = 
    ::irg::mesh::from_file(argv[1], mesh_shader);
  ::irg::texture tex{"./4/textures/marble_texture.jpg"};

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

        movement = updated;
        mesh_shader.set_uniform_matrix(
          "u_transform",
          ::glm::toMat4(::glm::angleAxis(::glm::radians(1.2f), diff))
            * mesh_shader.get_uniform_matrix("u_transform")
        );
      }

      return ::irg::ob::action::remain;
    }
  });

  glEnable(GL_DEPTH_TEST);

  ::irg::window_loop(window, [&] {
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    tex.use();
    mesh.draw();

    ::irg::assert_no_error();
  });
}