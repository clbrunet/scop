#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "scop/matrices/mat4.h"
#include "scop/matrices/mat4x1.h"
#include "scop/vectors/vec4.h"

void print_mat4(mat4_t mat4)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f", mat4[i][j]);
			if (j < 3) {
				printf(" ");
			}
		}
		printf("\n");
	}
}

void set_zero_mat4(mat4_t mat4)
{
	memset(mat4, 0, sizeof(mat4_t));
}

void set_identity_mat4(mat4_t mat4)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				mat4[i][j] = 1;
			} else {
				mat4[i][j] = 0;
			}
		}
	}
}

void set_orthographic_projection_mat4(mat4_t orthographic_mat4, GLfloat left, GLfloat right,
		GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
	assert(near >= 0 && far >= 0);

	set_zero_mat4(orthographic_mat4);

	orthographic_mat4[0][0] = 2 / (right - left);
	orthographic_mat4[0][3] = -(left + right) / (right - left);

	orthographic_mat4[1][1] = 2 / (top - bottom);
	orthographic_mat4[1][3] = -(bottom + top) / (top - bottom);

	orthographic_mat4[2][2] = -2 / (far - near);
	orthographic_mat4[2][3] = -(far + near) / (far - near);

	orthographic_mat4[3][3] = 1;
}


// horizontal_fov in radians
void set_perspective_projection_mat4(mat4_t projection_mat4, GLfloat horizontal_fov,
		GLfloat aspect_ratio, GLfloat near, GLfloat far)
{
	assert(!isnanf(horizontal_fov) && !isinff(horizontal_fov));
	assert(0 < horizontal_fov && horizontal_fov < 180);
	assert(near >= 0 && far >= 0);

	set_zero_mat4(projection_mat4);

	GLfloat projection_plane_distance = aspect_ratio / tan(horizontal_fov / 2);
	projection_mat4[0][0] = projection_plane_distance / aspect_ratio;
	projection_mat4[1][1] = projection_plane_distance;

	projection_mat4[2][2] = (near + far) / (near - far);
	projection_mat4[2][3] = (2 * far * near) / (near - far);

	projection_mat4[3][2] = -1;
}

// angle in radians
void set_yaw_mat4(mat4_t yaw_mat4, GLfloat angle)
{
	assert(!isnanf(angle) && !isinff(angle));

	set_identity_mat4(yaw_mat4);
	yaw_mat4[0][0] = cos(angle);
	yaw_mat4[0][2] = sin(angle);
	yaw_mat4[2][0] = -sin(angle);
	yaw_mat4[2][2] = cos(angle);
}

// angle in radians
void set_pitch_mat4(mat4_t pitch_mat4, GLfloat angle)
{
	assert(angle != NAN && angle != INFINITY);

	set_identity_mat4(pitch_mat4);
	pitch_mat4[1][1] = cos(angle);
	pitch_mat4[1][2] = -sin(angle);
	pitch_mat4[2][1] = sin(angle);
	pitch_mat4[2][2] = cos(angle);
}

// angle in radians
void set_roll_mat4(mat4_t roll_mat4, GLfloat angle)
{
	assert(!isnanf(angle) && !isinff(angle));

	set_identity_mat4(roll_mat4);
	roll_mat4[0][0] = cos(angle);
	roll_mat4[0][1] = -sin(angle);
	roll_mat4[1][0] = sin(angle);
	roll_mat4[1][1] = cos(angle);
}

void mat4_multiplication(mat4_t lhs, mat4_t rhs, mat4_t result)
{
	set_zero_mat4(result);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result[i][j] += lhs[i][k] * rhs[k][j];
			}
		}
	}
}

vec4_t mat4_vec4_multiplication(mat4_t mat4, vec4_t vec4)
{
	mat4x1_t mat4x1;
	vec4_to_mat4x1(vec4, mat4x1);
	mat4x1_t result_mat4x1 = { { 0 } };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result_mat4x1[i][0] += mat4[i][j] * mat4x1[j][0];
		}
	}
	return mat4x1_to_vec4(result_mat4x1);

}
