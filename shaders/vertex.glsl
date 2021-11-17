#version 400 core

layout (location = 0) in vec3 position;

uniform float x_displacement;

void main()
{
  gl_Position = vec4(position.x + x_displacement, position.yz, 1.0);
}
