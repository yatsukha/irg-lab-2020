#pragma once

#include <iostream> // TODO: delet

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace irg {

  struct point {
    float x;
    float y;
  };

  struct line_segment {
    point start;
    point end;
  };

  class line {
   public:
    virtual void draw() const = 0;
    virtual void update(line_segment const&) = 0;
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
  };

}