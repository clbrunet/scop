#include <math.h>
#include <stdio.h>

#include "scop/vec3.h"

void print_vec3(const vec3_t *vec3)
{
	printf("x: %f\n", vec3->x);
	printf("y: %f\n", vec3->y);
	printf("z: %f\n", vec3->z);
	printf("\n");
}

vec3_t vec3_addition(const vec3_t *lhs, const vec3_t *rhs)
{
	vec3_t result = {
		.x = lhs->x + rhs->x,
		.y = lhs->y + rhs->y,
		.z = lhs->z + rhs->z,
	};
	return result;
}

GLfloat vec3_magnitude(const vec3_t *vec3)
{
	return sqrt(vec3->x * vec3->x + vec3->y * vec3->y + vec3->z * vec3->z);
}

void vec3_normalize(vec3_t *vec3)
{
	GLfloat magnitude = vec3_magnitude(vec3);
	if (magnitude == 0) {
		return;
	}
	vec3->x /= magnitude;
	vec3->y /= magnitude;
	vec3->z /= magnitude;
}

void vec3_set_magnitude(vec3_t *vec3, GLfloat magnitude)
{
	vec3_normalize(vec3);
	vec3->x *= magnitude;
	vec3->y *= magnitude;
	vec3->z *= magnitude;
}
