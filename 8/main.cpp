#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <irg/common.hpp>
#include <irg/window.hpp>
#include <irg/texture.hpp>
#include <irg/fractals.hpp>
#include <irg/shader.hpp>
#include <irg/shape.hpp>
#include <irg/keyboard.hpp>

#include <GLFW/glfw3.h>

int main(int const argc, char** argv) {
  if (argc < 2) {
    ::std::cout << "Example usage: "
                << "\"-e 1000 -m 64 -u -2,2 -v -1.2,1.2 -c 0.11,0.6O\""
                << "\n"
                << "-c flag is optional, specify it to generate Julia set."
                << "\n";
    ::std::exit(EXIT_FAILURE);
  }

  auto configuration = ::irg::parse_configuration(argc, argv);
  configuration.screen_dim = {1200, 800};
  
  auto  guard  = ::irg::init();
  auto* window =
    ::irg::create_window(
      configuration.screen_dim.first, configuration.screen_dim.second);
  
  ::std::cout << "Generating fractal..." << "\n";
  
  auto fractal_gen = configuration.c
    ? ::irg::julia_from
    : ::irg::mandelbrot_from;
    
  
  ::irg::texture fractal_texture{
    fractal_gen(configuration).get(),
    static_cast<int>(configuration.screen_dim.first),
    static_cast<int>(configuration.screen_dim.second)};
    
  fractal_texture.use();
    
  ::std::cout << "Done." << "\n";
  ::std::cout << "Press R to regenerate the fractal if you changed window size."
              << "\n";
    
  ::irg::bind_events(window);
    
  ::irg::w_events.add_listener([&](auto&& w, auto&& h) {
    configuration.screen_dim = {w, h};
    return ::irg::ob::action::remain;
  });
    
  ::irg::k_events.add_listener([&](auto&& key, auto&& released) {
    if (key == GLFW_KEY_R && !released) {
      ::std::cout << "Regenerating..." << "\n";
      fractal_texture = {
        fractal_gen(configuration).get(),
        static_cast<int>(configuration.screen_dim.first),
        static_cast<int>(configuration.screen_dim.second)};
        
      fractal_texture.use();
      ::std::cout << "Done." << "\n";
    }
    return ::irg::ob::action::remain;
  });
    
  ::irg::polygon<4> rectangle{{
    {-1.f, -1.f},
    {-1.f, 1.f},
    {1.f, 1.f},
    {1.f, -1.f}
  }, ::irg::shader_program{
    {"shaders/vertex.glsl", GL_VERTEX_SHADER},
    {"shaders/fragment.glsl", GL_FRAGMENT_SHADER}
  }};

  ::irg::window_loop(window, [&]{
    rectangle.draw();
  });

}
