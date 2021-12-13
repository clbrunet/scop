#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "glad/glad.h"

#include "scop/app.h"
#include "scop/vectors/vec2.h"
#include "scop/vectors/vec3.h"

GLfloat radians(GLfloat degrees)
{
	assert(!isnanf(degrees) && !isinff(degrees));

	return degrees * (M_PI / 180);
}

GLfloat degrees(GLfloat radians)
{
	assert(!isnanf(radians) && !isinff(radians));

	return radians * (180 / M_PI);
}

vec3_t cross_product(const vec3_t *lhs, const vec3_t *rhs)
{
	return (vec3_t){
		.x = lhs->y * rhs->z - lhs->z * rhs->y,
		.y = lhs->z * rhs->x - lhs->x * rhs->z,
		.z = lhs->x * rhs->y - lhs->y * rhs->x,
	};
}

vec3_t get_triangle_normal(const vec3_t *p1, const vec3_t *p2, const vec3_t *p3)
{
	vec3_t edge1 = vec3_substraction(p2, p1);
	vec3_t edge2 = vec3_substraction(p3, p1);
	vec3_t normal = cross_product(&edge1, &edge2);
	vec3_normalize(&normal);
	return normal;
}

vec2_t get_texture_coordinates(const vec3_t *position, const vec3_t *normal,
		const bounding_box_t *model_bounding_box, GLfloat texture_aspect_ratio)
{
	vec2_t texture_coordinates = {};

	vec3_t abs_normal = {
		.x = fabsf(normal->x),
		.y = fabsf(normal->y),
		.z = fabsf(normal->z),
	};
	if (abs_normal.x > abs_normal.y && abs_normal.x > abs_normal.z) {
		texture_coordinates.u = (-position->z - model_bounding_box->z.min) / texture_aspect_ratio;
		texture_coordinates.v = position->y - model_bounding_box->y.min;
	} else if (abs_normal.y > abs_normal.x && abs_normal.y > abs_normal.z) {
		texture_coordinates.u = (position->x - model_bounding_box->x.min) / texture_aspect_ratio;
		texture_coordinates.v = -position->z - model_bounding_box->z.min;
	} else {
		texture_coordinates.u = (position->x - model_bounding_box->x.min) / texture_aspect_ratio;
		texture_coordinates.v = position->y - model_bounding_box->y.min;
	}
	return texture_coordinates;
}
