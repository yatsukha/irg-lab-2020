#version 330 core

out vec4 color;

in vec3 f_normal;
in vec3 f_pos;
in vec3 f_view_dir;

uniform sampler2D tex;
uniform vec3 light_color;
uniform vec3 light_position;
uniform int texture_present = 0;

void main() {
  float ambient = 0.4;
  float spec    = 0.5;

  vec3 object_color = vec3(0.9);

  vec3 norm = normalize(f_normal);

  vec3 light_direction = normalize(light_position - f_pos);
  vec3 reflect_direction = reflect(-light_direction, norm);

  vec3 diffuse = max(dot(norm, light_direction), 0.0) * light_color;
  float specular = spec * pow(max(dot(-f_view_dir, reflect_direction), 0.0), 128);

  vec4 c = vec4(1.0);

  if (texture_present == 1) {
    vec2 tex_coords = vec2(f_pos.x, f_pos.y + f_pos.z);
    if (tex_coords.y > 1.0)
      tex_coords.y -= 2.0;
    if (tex_coords.y < 1.0)
      tex_coords.y += 2.0;
    c = texture(tex, tex_coords);
  }

  color = c
    * vec4(((ambient + specular) * light_color + diffuse) * object_color, 1.0);
}