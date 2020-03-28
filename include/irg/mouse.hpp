#pragma once

#include <algorithm>
#include <vector>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irg/primitive.hpp>
#include <irg/obmodel.hpp>

namespace irg {

  namespace mouse_event_type {
    using on_start = ::std::function<void(point const&)>;
    using on_move  = ::std::function<void(line_segment const&)>;
    using on_final = ::std::function<ob::action(line_segment const&)>;
  }

  struct mouse_event_listener {
    mouse_event_type::on_start started;
    mouse_event_type::on_move  moved;
    mouse_event_type::on_final finalized;
  };

  class mouse_events : public ob::observer<mouse_event_listener> {
    bool tracking;
    line_segment ls;

   public:
    void static move_callback(GLFWwindow* w, double xpos, double ypos);
    void static click_callback(GLFWwindow* w, int btn, int type, int);
  };

  mouse_events extern m_events;


  /* class mouse_state { // closed namespace
   private:
    bool static tracking;
    line_segment static ls;

   public:
    ::std::function<void(line_segment&)> static on_move;
    ::std::function<void(line_segment&)> static on_finalize;

    void static move_callback(GLFWwindow* w, double xpos, double ypos) {
      if (!tracking) 
        return;

      int width, height;
      ::glfwGetWindowSize(w, &width, &height);

      ls.end = normalize({xpos, ypos}, {width, height});

      on_move(ls);
    }

    void static click_callback(GLFWwindow* w, int btn, int type, int) {
      if (btn != GLFW_MOUSE_BUTTON_LEFT || type != GLFW_PRESS)
        return;

      double xpos, ypos;
      ::glfwGetCursorPos(w, &xpos, &ypos);
      
      int width, height;
      ::glfwGetWindowSize(w, &width, &height);

      point p = normalize({xpos, ypos}, {width, height});

      if (!tracking)
        ls.start = p;
      else
        ls.end = p,
        on_finalize(ls);

      tracking = !tracking;
    }
  }; */

}