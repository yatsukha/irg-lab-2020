#version 330 core

out vec4 color;

in vec2 f_tex_coords;
in vec3 f_normal;
in vec3 f_pos;
in vec3 f_view_dir;

uniform sampler2D tex;
uniform vec3 light_color;
uniform vec3 light_position;

uniform int texture_present = 0;

uniform float k_ambient  = 0.4;
uniform float k_diffuse  = 1.0;
uniform float k_specular = 0.5;

void main() {
  vec3 norm = normalize(f_normal);

  vec3 light_direction = normalize(light_position - f_pos);
  vec3 reflect_direction = reflect(-light_direction, norm);

  vec3 diffuse = k_diffuse * max(dot(norm, light_direction), 0.0) * light_color;
  float specular = k_specular * pow(max(dot(-f_view_dir, reflect_direction), 0.0), 128);

  vec4 c = vec4(1.0);

  if (texture_present == 1) {
    c = texture(tex, vec2(f_pos.x, f_pos.y + f_pos.z));
  }
  
  vec3 base_object_color = vec3(0.9);

  color = c
    * vec4(((ambient + specular) * light_color + diffuse) * base_object_color, 1.0);
}