#ifndef SCOP_MAT4_H
#define SCOP_MAT4_H

#include "glad/glad.h"

typedef struct vec4_s vec4_t;

typedef GLfloat mat4_t[4][4];

void print_mat4(mat4_t mat4);

void set_zero_mat4(mat4_t mat4);
void set_identity_mat4(mat4_t mat4);

void set_perspective_projection_mat4(mat4_t projection_mat4, GLfloat horizontal_fov,
		GLfloat aspect_ratio, GLfloat near, GLfloat far);

void set_yaw_mat4(mat4_t yaw_mat4, GLfloat angle);
void set_pitch_mat4(mat4_t pitch_mat4, GLfloat angle);
void set_roll_mat4(mat4_t roll_mat4, GLfloat angle);

void mat4_multiplication(mat4_t lhs, mat4_t rhs, mat4_t result);
vec4_t mat4_vec4_multiplication(mat4_t mat4, vec4_t vec4);

#endif
