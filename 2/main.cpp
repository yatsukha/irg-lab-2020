#include <algorithm>
#include <vector>
#include <memory>
#include <unordered_set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irg/common.hpp>
#include <irg/shader.hpp>
#include <irg/mouse.hpp>
#include <irg/shape.hpp>

#define GUARD __FILE__##guard##__LINE__

int main() {
  auto  GUARD  = ::irg::init();
  auto* window = ::irg::create_window();

  auto line_shader_program = ::irg::shader_program(
    ::irg::shader("./2/vertex.glsl", GL_VERTEX_SHADER),
    ::irg::shader("./2/fragment.glsl", GL_FRAGMENT_SHADER)
  );

  ::std::vector<::std::shared_ptr<::irg::shape>> shapes;

  ::glfwSetMouseButtonCallback(window, ::irg::mouse_events::click_callback);
  ::glfwSetCursorPosCallback(window, ::irg::mouse_events::move_callback);

  ::glfwSetKeyCallback(window, ::irg::keyboard_events::callback);

  ::std::unordered_set<int> possible_shapes{
    GLFW_KEY_L, GLFW_KEY_T, GLFW_KEY_S, GLFW_KEY_P};
  int shape = GLFW_KEY_L;

  ::irg::k_events.add_listener([&](int const key) {
    if (key == GLFW_KEY_E && shapes.size())
      shapes.resize(shapes.size() - 1);
    else if (possible_shapes.count(key))
      shape = key;
    return ::irg::ob::action::remain;
  });

  ::irg::m_events.add_listener({
    [&] (auto const& point) {
      switch (shape) {
        case GLFW_KEY_L:
          shapes.emplace_back(
            new ::irg::line(point, line_shader_program, ::irg::m_events));
          shapes.emplace_back(new ::irg::bresenham_line(
            window, point, line_shader_program, ::irg::m_events));
          break;
        case GLFW_KEY_T:
          shapes.emplace_back(new ::irg::polygon<3>(
            point, line_shader_program, ::irg::m_events));
          break;
        case GLFW_KEY_S:
          shapes.emplace_back(new ::irg::polygon<4>(
            point, line_shader_program, ::irg::m_events));
          break;
        case GLFW_KEY_P:
          shapes.emplace_back(new ::irg::polygon<5>(
            point, line_shader_program, ::irg::m_events));
          break;
      }
    },
    {}, {}
  });

  ::irg::window_loop(window, [&] {
    glClearColor(.5f, .3f, .7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto const& shape : shapes)
      shape->draw();
  });
}