#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model = mat4(1.0);
uniform mat4 view  = mat4(1.0);
uniform mat4 screen_ratio = mat4(1.0);
uniform mat4 projection = mat4(1.0);

out vec3 f_pos;
out vec3 f_view;
out vec3 f_normal;

void main() {
  f_pos = pos;
  f_view = -vec3(view[0][2], view[1][2], view[2][2]);
  f_normal = normal;
  gl_Position = screen_ratio * projection * view * model * vec4(pos, 1.0);
}
