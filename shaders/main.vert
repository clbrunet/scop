#version 400 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

uniform mat4 projection_view_model;

out vec4 v_color;

void main()
{
  gl_Position = projection_view_model * position;
  v_color = color;
}
