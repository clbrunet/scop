#version 400 core

in vec4 v_color;
in vec2 v_texture_coordinates;

uniform vec4 model_color;
uniform sampler2D sampler;
uniform float texture_portion;

out vec4 out_color;

void main()
{
  out_color = mix((v_color * model_color), texture(sampler, v_texture_coordinates), texture_portion);
}
