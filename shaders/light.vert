#version 400 core

layout (location = 0) in vec4 position;

uniform mat4 projection_view;

void main()
{
  gl_PointSize = 30;
  gl_Position = projection_view * position;
}
