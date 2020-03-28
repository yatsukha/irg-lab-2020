#include <irg/keyboard.hpp>

namespace irg {

  keyboard_events k_events;

  void keyboard_events::callback(GLFWwindow*, int key, int, int action, int) {
    if (action != GLFW_PRESS)
      return;
    auto iter = k_events.listeners.begin();
    while (iter != k_events.listeners.end())
      if ((*iter)(key) == ob::action::detach)
        iter = k_events.listeners.erase(iter);
      else
        ++iter;
  }

}