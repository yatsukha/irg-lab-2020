#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 u_transform = mat4(1.0);

out vec2 tex_coords;

void main() {
  gl_Position = u_transform * vec4(pos, 1.0);
  tex_coords = vec2(pos.x, pos.y);
}