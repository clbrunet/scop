#version 400 core

layout (location = 0) in vec4 position;
layout (location = 2) in vec2 texture_coordinates;
layout (location = 3) in vec3 normal;

uniform mat4 view_model;
uniform mat4 projection_view_model;

out vec2 v_texture_coordinates;
out vec3 v_normal;
out vec3 v_frag_position;

void main()
{
  gl_Position = projection_view_model * position;

  v_texture_coordinates = texture_coordinates;
  v_normal = normalize(mat3(view_model) * normal);
  v_frag_position = vec3(view_model * position);
}
