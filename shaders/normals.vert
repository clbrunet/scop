#version 400 core

layout (location = 0) in vec4 position;
layout (location = 3) in vec3 normal;

uniform mat4 view_model;

out vec3 v_normal;

void main()
{
  gl_Position = view_model * position;

  v_normal = normalize(mat3(view_model) * normal);
}
