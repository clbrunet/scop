#ifndef SCOP_VEC2_H
#define SCOP_VEC2_H

#include "glad/glad.h"

typedef struct vec4_s vec4_t;

typedef struct vec2_s {
	GLfloat x;
	GLfloat y;
}	vec2_t;

void print_vec2(const vec2_t *vec2);

vec4_t vec2_to_vec4(const vec2_t *vec2);

#endif
