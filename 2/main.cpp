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

  ::irg::simple_line sl = ::irg::line_segment{{0, 2}, {0, 2}};

  ::irg::mouse_state::on_move = ::std::bind(
    &::irg::simple_line::update, &sl, ::std::placeholders::_1);

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

    sl.draw();
  });
}