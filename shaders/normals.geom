#version 400 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in vec3 v_normal[];

uniform mat4 projection;

void main()
{
  vec4 center = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
  gl_Position = projection * center;
  EmitVertex();
  gl_Position = projection * (center + 0.3 * vec4(v_normal[0], 0));
  EmitVertex();
  EndPrimitive();
}
