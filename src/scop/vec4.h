#ifndef SCOP_VEC4_H
#define SCOP_VEC4_H

#include "glad/glad.h"

typedef GLfloat mat4x1_t[4][1];

typedef struct vec4_s {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat w;
}	vec4_t;

void print_vec4(vec4_t vec4);
void vec4_to_mat4x1(vec4_t vec4, mat4x1_t mat4x1);

#endif
