#pragma once

#include <iostream> // TODO: delet
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace irg {

  struct point {
    float x;
    float y;
  };

  inline point normalize(point const& p, point const& dim) noexcept {
    return {p.x / dim.x * 2.0 - 1.0, -p.y / dim.y * 2.0 + 1.0};
  }

  inline point scale(point const& p, point const& dim) noexcept {
    return {(p.x + 1.0) / 2.0 * dim.x, (p.y - 1.0) / 2.0 * dim.y * -1.0};
  }

  struct line_segment {
    point start;
    point end;
  };

  class line {
   public:
    virtual void draw() const = 0;
    virtual void update(line_segment const&) = 0;

    virtual ~line() = default;
  };

  class simple_line : public line {
   private:
    unsigned VAO;
    unsigned VBO;

   public:
    simple_line(line_segment const& ls) {
      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      glBufferData(GL_ARRAY_BUFFER, sizeof(ls), 
                   reinterpret_cast<float const*>(&ls), GL_DYNAMIC_DRAW);

      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(point), nullptr);
      glEnableVertexAttribArray(0);
    }

    void draw() const override {
      glBindVertexArray(VAO);
      glDrawArrays(GL_LINES, 0, 2);
    }

    void update(line_segment const& ls) override {
      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(line_segment), 
                   reinterpret_cast<float const*>(&ls), GL_DYNAMIC_DRAW);
    }

    ~simple_line() {
      glDeleteBuffers(1, &VBO);
      glDeleteVertexArrays(1, &VAO);
    }
  };

  class bresenham_line : public line {
   private:
    GLFWwindow* w;
    line_segment ls;

   public:
    bresenham_line(GLFWwindow* w, line_segment const& ls): ls(ls), w(w) {}
    
    void draw() const override {
      int width, height;
      ::glfwGetWindowSize(w, &width, &height);
      point dim = {width, height};

      line_segment n = {scale(ls.start, dim), scale(ls.end, dim)};

      float db = (n.end.y - n.start.y) / (n.end.x - n.start.x);
      bool swapped = ::std::abs(db) >= 1.0;
      ::std::vector<point> points;

      if (swapped)
        ::std::swap(n.start.x, n.start.y),
        ::std::swap(n.end.x, n.end.y),
        db = 1.0 / db;

      if (n.start.x > n.end.x) {
        point tmp = n.start;
        n.start = n.end;
        n.end = tmp;
      }

      float ydelta = db >= 0.0 ? 1.0 : -1.0;
      float d = db - ydelta / 2.0;
      float y = n.start.y;

      for (float x = n.start.x; x <= n.end.x; x += 1.0) {
        points.push_back(normalize(swapped ? point{y, x} : point{x, y}, dim));
        if (ydelta * d >= 0.0)
          y += ydelta,
          d -= ydelta;
        d += db;
      }


      /* if (::std::abs(db) < 1.0) {// only small code duplication :^)
        if (n.start.x > n.end.x) {
          point tmp = n.start;
          n.start = n.end;
          n.end = tmp;
        }

        float ydelta = db >= 0.0 ? 1.0 : -1.0;
        float d = db - ydelta / 2.0;
        float y = n.start.y;
        for (float x = n.start.x; x <= n.end.x; x += 1.0) {
          points.push_back(normalize({x, y}, dim));
          if (ydelta * d >= 0.0)
            y += ydelta,
            d -= ydelta;
          d += db;
        }
      } else {
        if (n.start.y > n.end.y) {
          point tmp = n.start;
          n.start = n.end;
          n.end = tmp;
        }

        db = 1.0 / db;
        float xdelta = db >= 0.0 ? 1.0 : -1.0;
        float d = db - xdelta / 2.0;
        float x = n.start.x;
        for (float y = n.start.y; y <= n.end.y; y += 1.0) {
          points.push_back(normalize({x, y}, dim));
          if (xdelta * d >= 0.0)
            x += xdelta,
            d -= xdelta;
          d += db;
        }
      } */
      

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
    }

    void update(line_segment const& ls) override {
      this->ls = ls;
    }
  };

}