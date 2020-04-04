#version 330 core

out vec4 color;

in vec2 tex_coords;
in vec3 f_normal;
in vec3 f_pos;
in vec3 f_view_pos;

uniform sampler2D tex;
uniform vec3 light_color;
uniform vec3 light_position;

void main() {
  float ambient = 0.4;
  float spec    = 0.5;

  vec3 object_color = vec3(0.9);

  vec3 norm = normalize(f_normal);
  vec3 view_direction = normalize(f_view_pos - f_pos);

  vec3 light_direction = normalize(light_position - f_pos);
  vec3 reflect_direction = reflect(-light_direction, norm);

  vec3 diffuse = max(dot(norm, light_direction), 0.0) * light_color;
  float specular = spec * pow(max(dot(view_direction, reflect_direction), 0.0), 32);

  color = texture(tex, tex_coords) 
    * vec4(((ambient + specular) * light_color + diffuse) * object_color, 1.0);
}