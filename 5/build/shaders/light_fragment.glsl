#version 330 core

out vec4 color;

in vec3 f_pos;
in vec3 f_view;
in vec3 f_normal;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);

void main() {
  float angle = dot(normalize(-f_view), normalize(f_normal));
  color = vec4(light_color, 1.0) * angle + vec4(1.0) * (1.0 - angle);
}