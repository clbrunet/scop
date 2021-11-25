#include <stdio.h>

#include "scop/vectors/vec4.h"
#include "scop/vectors/vec3.h"
#include "scop/matrices/mat4x1.h"

void print_vec4(vec4_t vec4)
{
	printf("x: %f\n", vec4.x);
	printf("y: %f\n", vec4.y);
	printf("z: %f\n", vec4.z);
	printf("w: %f\n", vec4.w);
	printf("\n");
}

vec3_t vec4_to_vec3(const vec4_t *vec4)
{
	vec3_t vec3 = {
		.x = vec4->x,
		.y = vec4->y,
		.z = vec4->z,
	};
	return vec3;
}

void vec4_to_mat4x1(vec4_t vec4, mat4x1_t mat4x1)
{
	mat4x1[0][0] = vec4.x;
	mat4x1[1][0] = vec4.y;
	mat4x1[2][0] = vec4.z;
	mat4x1[3][0] = vec4.w;
}
