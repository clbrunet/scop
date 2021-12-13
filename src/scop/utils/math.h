#ifndef SCOP_UTILS_MATH_H
#define SCOP_UTILS_MATH_H

#include "glad/glad.h"

typedef struct vec3_s vec3_t;
typedef struct vec2_s vec2_t;
typedef struct bounding_box_s bounding_box_t;

GLfloat radians(GLfloat degrees);
GLfloat degrees(GLfloat radians);

vec3_t cross_product(const vec3_t *lhs, const vec3_t *rhs);
vec3_t get_triangle_normal(const vec3_t *p1, const vec3_t *p2, const vec3_t *p3);
vec2_t get_texture_coordinates(const vec3_t *position, const vec3_t *normal,
		const bounding_box_t *model_bounding_box, GLfloat texture_aspect_ratio);

#endif
