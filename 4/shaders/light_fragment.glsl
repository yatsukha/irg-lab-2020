#version 330 core

out vec4 color;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);

void main() {
  color = vec4(light_color, 1.0);
}