#include "scop/matrices/mat4x1.h"
#include "scop/vectors/vec4.h"

vec4_t mat4x1_to_vec4(mat4x1_t mat4x1)
{
	vec4_t vec4;

	vec4.x = mat4x1[0][0];
	vec4.y = mat4x1[1][0];
	vec4.z = mat4x1[2][0];
	vec4.w = mat4x1[3][0];
	return vec4;
}
