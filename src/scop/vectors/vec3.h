#ifndef SCOP_VECTORS_VEC3_H
#define SCOP_VECTORS_VEC3_H

#include "glad/glad.h"

typedef struct vec4_s vec4_t;

typedef struct vec3_s {
	GLfloat x;
	GLfloat y;
	GLfloat z;
}	vec3_t;

vec3_t vec3(GLfloat x, GLfloat y, GLfloat z);

void print_vec3(const vec3_t *vec3);

vec4_t vec3_to_vec4(const vec3_t *vec3);

vec3_t vec3_addition(const vec3_t *lhs, const vec3_t *rhs);

GLfloat vec3_magnitude(const vec3_t *vec3);
void vec3_normalize(vec3_t *vec3);
void vec3_set_magnitude(vec3_t *vec3, GLfloat magnitude);

#endif
