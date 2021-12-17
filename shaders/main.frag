#version 400 core

in vec4 v_color;
in vec2 v_texture_coordinates;

uniform float texture_portion;
uniform sampler2D sampler;

out vec4 out_color;

void main()
{
  out_color = mix(v_color, texture(sampler, v_texture_coordinates), texture_portion);
}
