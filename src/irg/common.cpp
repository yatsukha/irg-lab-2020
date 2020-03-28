#include <irg/common.hpp>

namespace irg {

  void terminate(char const* err) {
    ::std::cerr << err << "\n";
    ::std::exit(EXIT_FAILURE);
  }

  void assert_no_error() {
    if (auto err = glGetError(); err) {
      ::std::cerr << ::std::hex << err << "\n";
      terminate("Failed error assertion.");
    }
  }

  on_scope_exit init(int const major_version, int const minor_version) {
    ::glfwInit();
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    ::glfwSwapInterval(1);

    return on_scope_exit{::glfwTerminate};
  }

  ::GLFWwindow* create_window(int const width, int const height) {
    ::GLFWwindow* w = ::glfwCreateWindow(width, height, "", nullptr, nullptr);

    if (!w)
      terminate("Unable to create a window.");

    ::glfwMakeContextCurrent(w);
    ::glfwSetWindowPos(w, 100, 100);

    if (!::gladLoadGLLoader(
          reinterpret_cast<::GLADloadproc>(::glfwGetProcAddress)))
      terminate("Unable to initialize GLAD.");

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback( // resizing
      w, [](auto*, auto const w, auto const h) { glViewport(0, 0, w, h); });

    return w;
  }

  namespace detail {
    void default_inputs(::GLFWwindow* window) {
      if (::glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        ::glfwSetWindowShouldClose(window, true);
    }
  }

  void window_loop(::GLFWwindow* window, ::std::function<void(void)> render) {
    while (!::glfwWindowShouldClose(window)) {
      detail::default_inputs(window);

      render();

      ::glfwPollEvents();
      ::glfwSwapBuffers(window);
    }
  }

}