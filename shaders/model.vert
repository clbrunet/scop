#version 400 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texture_coordinates;

uniform mat4 projection_view_model;

out vec4 v_color;
out vec2 v_texture_coordinates;

void main()
{
  gl_Position = projection_view_model * position;

  v_color = color;
  v_texture_coordinates = texture_coordinates;
}
