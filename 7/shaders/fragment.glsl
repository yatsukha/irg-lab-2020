#version 330 core

out vec4 color;

in vec2 f_tex_coords;
in vec3 f_normal;
in vec3 f_pos;
in vec3 f_view_dir;
in vec3 f_interp_normal;

uniform sampler2D tex;
uniform vec3 light_color;
uniform vec3 light_position;

uniform int texture_present = 0;
uniform int gouraud_shading = 0;

uniform float k_ambient   = 0.4;
uniform float k_diffuse   = 1.0;
uniform float k_specular  = 0.5;
uniform float k_roughness = 128.0;

void main() {
  vec3 norm   = normalize(f_normal);
  vec3 i_norm = normalize(f_interp_normal);

  vec3 light_direction = normalize(light_position - f_pos);
  float diffuse = k_diffuse * max(dot(i_norm, light_direction), 0.0);
  vec3 base_object_color = vec3(0.9);

  if (texture_present == 1) {
    color = texture(tex, vec2(f_pos.x, f_pos.y + f_pos.z));
  } else {
    color = vec4(1.0, 1.0, 1.0, 1.0);
  }
  
  vec3 reflect_direction = reflect(-light_direction, norm);
  
  float specular = 0.0;
  
  if (gouraud_shading == 0)
    specular = k_specular *
      pow(
        max(dot(-f_view_dir, reflect_direction), 0.0),
        k_roughness);
  
  color *=
    vec4(
      (
        (k_ambient + diffuse + specular) * light_color
      ) * base_object_color,
      1.0);
}