#ifndef SCOP_VEC3_H
#define SCOP_VEC3_H

#include "glad/glad.h"

typedef struct vec3_s {
	GLfloat x;
	GLfloat y;
	GLfloat z;
}	vec3_t;

void print_vec3(vec3_t vec3);

#endif
