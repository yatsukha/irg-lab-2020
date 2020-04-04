#include <irg/shape.hpp>

namespace irg {

  void line::update_buffer(line_segment const& ls) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_segment), 
                  reinterpret_cast<float const*>(&ls), GL_DYNAMIC_DRAW);
  }

  line::line(point const start, shader_program const sp, mouse_events& me)
    : shape(sp)
  {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    unit const arr[] = {start.x, start.y, start.x, start.y};

    glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(point), nullptr);
    glEnableVertexAttribArray(0);

    me.add_listener({
      {}, // on start
      ::std::bind(&line::update_buffer, this, ::std::placeholders::_1), // on move
      [this](auto const& ls) { // on final
        shape::locked = true;
        update_buffer(ls);
        return ob::action::detach;
      }
    });
  }

  void line::draw() const {
    shape::draw();
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
  }

  line::~line() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
  }

  //

  bresenham_line::bresenham_line(GLFWwindow* w, point const start, 
                                 shader_program const sp, mouse_events& me)
    : w(w), ls(adjust({start, start})), shape(sp)
  {
    me.add_listener({
      {},
      [this](auto const& ls) { 
        this->ls = adjust(ls);
      },
      [this](auto const& ls) {
        shape::locked = true;
        this->ls = adjust(ls); 
        return ob::action::detach;
      }
    });
  }

  void bresenham_line::draw() const {
    shape::draw();

    int width, height;
    ::glfwGetWindowSize(w, &width, &height);
    point dim = {static_cast<float>(width), static_cast<float>(height)};

    line_segment n = {scale(ls.start, dim), scale(ls.end, dim)};

    float db = (n.end.y - n.start.y) / (n.end.x - n.start.x);
    bool swapped = ::std::abs(db) >= 1.0;
    ::std::vector<point> points;

    using ::std::swap;

    if (swapped)
      swap(n.start.x, n.start.y),
      swap(n.end.x, n.end.y),
      db = 1.0 / db;

    if (n.start.x > n.end.x)
      swap(n.start, n.end);

    float ydelta = db >= 0.0 ? 1.0 : -1.0;
    float d = db - ydelta / 2.0;
    float y = n.start.y;

    for (float x = n.start.x; x < n.end.x; x += 1.0) {
      points.push_back(normalize(swapped ? point{y, x} : point{x, y}, dim));
      if (ydelta * d >= 0.0)
        y += ydelta, 
        d -= ydelta;
      d += db;
    }

    unsigned VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
      GL_ARRAY_BUFFER, sizeof(point) * points.size(),
      reinterpret_cast<float const*>(points.data()), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(point), nullptr);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, points.size());

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }

  //

  scanline_polygon::scanline_polygon(point const& p, shader_program const sp, 
                                     mouse_events& me, keyboard_events& ke)
    : shape(sp), vertices({homogenous(p), homogenous(p)}) 
  {
    me.add_listener({
      {},
      [this](auto const& ls) {
        if (!last_point)
          vertices.back() = homogenous(ls.end);
        else
          shape::locked = true;
      },
      [this](auto const& ls) {
        if (shape::locked)
          return ob::action::remain;

        vertices.back() = homogenous(ls.end);
        vertices.push_back(vertices.back());

        return ob::action::remain;
      }
    });

    ke.add_listener([this](int const key) {
      if (key == GLFW_KEY_F) {
        this->last_point = true;
        this->assert_clockwise();
        return ob::action::detach;
      } else {
        return ob::action::remain;
      }
    });
  }

  void scanline_polygon::assert_clockwise() {
    ::glm::vec3 anchor{10.0f, 10.0f, 1.0f};

    auto clockwise = true;
    auto n = vertices.size();

    for (auto i = 0; clockwise && i < n; ++i)
      if (::glm::dot(
            vertices[(i + 2) % n], 
            ::glm::cross(vertices[i], vertices[(i + 1) % n])
          ) > 0.0f)
        clockwise = false;
    
    if (!clockwise)
      ::std::cout << "Polygon is either not clockwise or not convex." 
                  << "\n",
      ::std::reverse(vertices.begin(), vertices.end());
  }

  bool scanline_polygon::is_inside(point const& p) {
    auto sz = vertices.size();
    auto hp = homogenous(p);

    for (auto i = 0; i < sz; ++i)
      if (::glm::dot(hp, ::glm::cross(vertices[i], vertices[(i + 1) % sz])) > 0)
        return false;

    return true;
  }

  void scanline_polygon::draw() const {
    shape::draw();

    unsigned VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    on_scope_exit guard{[&]{
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
    }};

    glBufferData(
      GL_ARRAY_BUFFER, sizeof(::glm::vec3) * vertices.size(),
      reinterpret_cast<float const*>(vertices.data()), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(::glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    glDrawArrays(shape::locked ? GL_LINE_LOOP : GL_LINE_STRIP, 0, vertices.size());
    assert_no_error();
  }
  

}