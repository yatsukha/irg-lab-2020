#version 330 core

layout (location = 0) in vec2 pos;

out vec2 f_tex_pos;

void main() {
  gl_Position = vec4(pos, 0.0, 1.0);
  f_tex_pos = vec2((pos.x + 1.0) / 2.0, (pos.y + 1.0) / 2.0);
}