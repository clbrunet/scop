#version 400 core

struct material_t
{
  vec3 diffuse;
  sampler2D diffuse_maps;
  float shininess;
};

struct light_t
{
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec2 v_texture_coordinates;
in vec3 v_normal;
in vec3 v_frag_position;

uniform material_t material;
uniform light_t light;
uniform float texture_portion;

out vec4 out_color;

void main()
{
  vec3 color = mix(material.diffuse, vec3(texture(material.diffuse_maps,
          v_texture_coordinates)), texture_portion);

  vec3 frag_to_light = normalize(light.position - v_frag_position);

  float diffuse_strength = max(dot(v_normal, frag_to_light), 0);
  vec3 diffuse = diffuse_strength * light.diffuse;

  vec3 halfway = normalize(frag_to_light + normalize(-v_frag_position));
  float specular_strength = pow(max(dot(v_normal, halfway), 0), material.shininess);
  vec3 specular = specular_strength * light.specular;

  out_color = vec4((light.ambient + diffuse + specular) * color, 1);
}
