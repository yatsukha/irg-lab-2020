#include <algorithm>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irg/common.hpp>
#include <irg/shader.hpp>
#include <irg/mouse.hpp>
#include <irg/primitive.hpp>

#define GUARD __FILE__##guard##__LINE__

int main() {
  auto  GUARD  = ::irg::init();
  auto* window = ::irg::create_window();

  ::std::vector<::std::shared_ptr<::irg::line>> lines;

  lines.emplace_back(new ::irg::simple_line({{0, 2}, {0, 2}}));
  lines.emplace_back(new ::irg::bresenham_line(window, {{0, 2}, {0, 2}}));

  ::irg::mouse_state::on_move = [&lines](auto const& data) {
    lines[lines.size() - 2]->update(data);
    ::irg::line_segment offset = data;
    offset.start.y -= 0.02;
    offset.end.y -= 0.02;
    lines[lines.size() - 1]->update(offset);
  };

  ::irg::mouse_state::on_finalize = [&](auto const& data) {
    lines.emplace_back(new ::irg::simple_line({{0, 2}, {0, 2}}));
    lines.emplace_back(new ::irg::bresenham_line(window, {{0, 2}, {0, 2}}));
  };

  ::glfwSetMouseButtonCallback(window, ::irg::mouse_state::click_callback);
  ::glfwSetCursorPosCallback(window, ::irg::mouse_state::move_callback);

  auto line_shader_program = ::irg::shader_program(
    ::irg::shader("./2/vertex.glsl", GL_VERTEX_SHADER),
    ::irg::shader("./2/fragment.glsl", GL_FRAGMENT_SHADER)
  );

  ::irg::window_loop(window, [&] {
    line_shader_program.activate();

    glClearColor(.5f, .3f, .7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto const& line : lines)
      line->draw();  
  });
}