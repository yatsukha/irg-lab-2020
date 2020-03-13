#include <irg/mouse.hpp>

namespace irg {

  bool mouse_state::tracking = false;

  line_segment mouse_state::ls = {{-1.1f, -1.1f}, {-1.1f, -1.1f}};

  ::std::function<void(line_segment&)> mouse_state::on_move = 
    [](auto) {};

  ::std::function<void(line_segment&)> mouse_state::on_finalize = 
    [](auto) {};

}