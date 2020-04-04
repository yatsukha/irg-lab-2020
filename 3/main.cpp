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

  auto shader_program = ::irg::shader_program(
    ::irg::shader("./3/vertex.glsl", GL_VERTEX_SHADER),
    ::irg::shader("./3/fragment.glsl", GL_FRAGMENT_SHADER)
  );

  ::irg::bind_events(window);

  ::std::shared_ptr<::irg::scanline_polygon> sp;
  bool point_tested = false;

  ::irg::m_events.add_listener({
    [&](auto const& point) {
      if (!sp)
        sp.reset(
          new ::irg::scanline_polygon(
            point, shader_program, ::irg::m_events, ::irg::k_events
          )
        );
    },
    {}, {}
  });

  ::std::cout << "Draw a convex polygon on the screen by clicking." << "\n";
  ::std::cout << "Press F if you wish to complete the polygon and connect "
              << "the first and last point." << "\n";
  ::std::cout << "If the polygon is not convex the program will not "
              << "work properly." << "\n";

  ::irg::window_loop(window, [&] {
    glClearColor(.5f, .3f, .7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!point_tested && sp && sp->is_final()) {
      ::std::cout << "Please pick a point by clicking on the screen." << "\n";

      ::irg::m_events.add_listener({
        {}, {},
        [&sp](auto const& ls) { 
          ::std::cout << "Point is inside polygon: " << sp->is_inside(ls.end)
                      << "\n";
          sp->set_filled(true);
          return ::irg::ob::action::detach; 
        }
      });

      point_tested = true;
    }

    if (sp)
      sp->draw();
  });
}