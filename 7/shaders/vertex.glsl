#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec3 A;
layout (location = 3) in vec3 triangle_A;
layout (location = 4) in vec3 B;
layout (location = 5) in vec3 triangle_B;
layout (location = 6) in vec3 C;
layout (location = 7) in vec3 triangle_C;

uniform int gouraud_shading = 0;

uniform mat4 model = mat4(1.0);
uniform mat4 view  = mat4(1.0);
uniform mat4 projection = mat4(1.0);

out vec2 f_tex_coords;
out vec3 f_normal;
out vec3 f_pos;
out vec3 f_view_dir;
out vec3 f_interp_normal;

void main() {
  vec4 tmp = model * vec4(pos, 1.0);

  f_tex_coords = vec2(pos.x, pos.y);

  gl_Position = projection * view * tmp;

  f_pos = vec3(tmp);
  f_normal = mat3(transpose(inverse(model))) * vec3(normal);

  f_view_dir = -vec3(view[0][2], view[1][2], view[2][2]);
  
  if (gouraud_shading == 0) {
    f_interp_normal = f_normal;
  } else {
    float d  = length(cross(B - A, C - A));
    float t1 = length(cross(B - pos, C - pos)) / d;
    float t2 = length(cross(A - pos, C - pos)) / d;
    float t3 = 1.0 - t2 - t1;
    
    f_interp_normal = t1 * normalize(triangle_A) +
                      t2 * normalize(triangle_B) +
                      t3 * normalize(triangle_C);

    if (d == 0.0)
      f_interp_normal = f_normal;
  }
}
