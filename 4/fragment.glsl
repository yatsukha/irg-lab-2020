#version 330 core

out vec4 color;

in vec2 tex_coords;

uniform sampler2D tex;

void main() {
  color = texture(tex, tex_coords) * vec4(0.9, 0.9, 0.9, 1.0);
}