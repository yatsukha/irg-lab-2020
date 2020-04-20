#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model = mat4(1.0);
uniform mat4 view  = mat4(1.0);
uniform mat4 screen_ratio = mat4(1.0);

out vec2 f_tex_coords;
out vec3 f_normal;
out vec3 f_pos;
out vec3 f_view_dir;

void main() {
  vec4 tmp = model * vec4(pos, 1.0);

  f_tex_coords = vec2(pos.x, pos.y);

  gl_Position = screen_ratio * view * tmp;

  f_pos = vec3(tmp);
  f_normal = mat3(transpose(inverse(model))) * normal;

  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa
  f_view_dir = vec3(view[0][2], view[1][2], view[2][2]);
}