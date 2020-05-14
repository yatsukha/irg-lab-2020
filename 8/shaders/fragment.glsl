#version 330 core

in vec2 f_tex_pos;

out vec4 color;

uniform sampler2D tex;

void main() {
  color = texture(tex, f_tex_pos);
}