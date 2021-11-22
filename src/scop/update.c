#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/mat4.h"
#include "scop/utils.h"

void set_zero_mat4(mat4_t mat4)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat4[i][j] = 0;
		}
	}
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

// horizontal_fov in radians
void set_perspective_projection_mat4(mat4_t projection_mat4, GLfloat horizontal_fov,
		GLfloat aspect_ratio)
{
	set_zero_mat4(projection_mat4);
	GLfloat projection_plane_distance = aspect_ratio / tan(horizontal_fov / 2);
	projection_mat4[0][0] = projection_plane_distance / aspect_ratio;
	projection_mat4[1][1] = projection_plane_distance;
	projection_mat4[2][2] = -1;
	projection_mat4[3][2] = -1;
}

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

void print_vec4(GLfloat vec4[4][1])
{
	for (int i = 0; i < 4; i++) {
		printf("%f", vec4[i][0]);
		printf("\n");
	}
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

void mat4_vec4_multiplication(mat4_t mat4, GLfloat vec4[4][1], GLfloat result[4][1])
{
	for (int i = 0; i < 4; i++) {
		result[i][0] = 0;
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][0] += mat4[i][j] * vec4[j][0];
		}
	}
}

void debug(app_t *app, mat4_t final)
{
	(void)app;
	(void)final;
	static int is_first = 1;
	if (is_first) {
		// print_mat4(final);
		// printf("\n");

		// GLfloat vec4[4][1] = {
		// 	{ 0.000911, },
		// 	{ -0.001444, },
		// 	{ 0.866024, },
		// 	{ 1, },
		// };
		GLfloat vec4[4][1] = {
			{ 0.707182, },
			{ -0.407108, },
			{ -0.290098, },
			{ 1, },
		};

		GLfloat result[4][1];
		mat4_vec4_multiplication(final, vec4, result);
		// print_vec4(result);

		is_first = 0;
	}
}

void update(app_t *app, GLdouble time, GLdouble delta_time)
{
	(void)time; (void)delta_time;
	glClear(GL_COLOR_BUFFER_BIT);

	mat4_t view_mat4 = {
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, -4 },
		{ 0, 0, 0, 1 },
	};

	mat4_t projection_mat4;
	set_perspective_projection_mat4(projection_mat4, radians(60),
			(GLfloat)app->window_width / (GLfloat)app->window_height);

	mat4_t projection_view_mat4;
	mat4_multiplication(projection_mat4, view_mat4, projection_view_mat4);

	GLuint projection_view_uniform_location = glGetUniformLocation(app->program, "projection_view");
	assert(projection_view_uniform_location != (GLuint)-1);
	glUniformMatrix4fv(projection_view_uniform_location, 1, GL_TRUE, (const GLfloat *)projection_view_mat4);

	debug(app, projection_view_mat4);

	GLuint color_uniform_location = glGetUniformLocation(app->program, "color");
	assert(color_uniform_location != (GLuint)-1);

	GLfloat white = 1;
	for (GLsizei i = 0; i < app->triangle_count; i++) {
		glUniform4f(color_uniform_location, white, white, white, 1);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid *)(i * 3 * sizeof(GLuint)));
		white -= 0.08;
		if (white < 0.2) {
			white = 1;
		}
	}
}

