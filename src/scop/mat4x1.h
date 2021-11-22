#ifndef SCOP_MAT4X1_H
#define SCOP_MAT4X1_H

#include "glad/glad.h"

typedef struct vec4_s vec4_t;

typedef GLfloat mat4x1_t[4][1];

vec4_t mat4x1_to_vec4(mat4x1_t mat4x1);

#endif
