#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model = mat4(1.0);
uniform mat4 view  = mat4(1.0);

void main() {
  gl_Position = view * model * vec4(pos, 1.0);
}