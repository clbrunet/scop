#include <math.h>
#include <stdio.h>

#include "scop/vectors/vec3.h"
#include "scop/vectors/vec4.h"

vec3_t vec3(GLfloat x, GLfloat y, GLfloat z)
{
	return (vec3_t){
		.x = x,
		.y = y,
		.z = z,
	};
}

void print_vec3(const vec3_t *vec3)
{
	printf("x: %f\n", vec3->x);
	printf("y: %f\n", vec3->y);
	printf("z: %f\n", vec3->z);
}

vec4_t vec3_to_vec4(const vec3_t *vec3)
{
	return (vec4_t){
		.x = vec3->x,
		.y = vec3->y,
		.z = vec3->z,
		.w = 1,
	};
}

vec3_t vec3_addition(const vec3_t *lhs, const vec3_t *rhs)
{
	return (vec3_t){
		.x = lhs->x + rhs->x,
		.y = lhs->y + rhs->y,
		.z = lhs->z + rhs->z,
	};
}

vec3_t vec3_substraction(const vec3_t *lhs, const vec3_t *rhs)
{
	return (vec3_t){
		.x = lhs->x - rhs->x,
		.y = lhs->y - rhs->y,
		.z = lhs->z - rhs->z,
	};
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
