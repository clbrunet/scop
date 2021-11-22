#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/mat4.h"
#include "scop/vec4.h"
#include "scop/utils.h"

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
		vec4_t vec4 = {
			0.707182,
			-0.407108,
			-0.290098,
			1,
		};

		vec4_t result = mat4_vec4_multiplication(final, vec4);
		// print_vec4(result);
		(void)result;

		is_first = 0;
	}
}

void update(app_t *app, GLdouble time, GLdouble delta_time)
{
	(void)time; (void)delta_time;
	glClear(GL_COLOR_BUFFER_BIT);

	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(time * 20));
	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(time * 20));
	mat4_t roll_mat4;
	set_roll_mat4(roll_mat4, radians(time * 20));
	mat4_t pitch_yaw_mat4;
	mat4_multiplication(pitch_mat4, yaw_mat4, pitch_yaw_mat4);
	mat4_t model_mat4;
	mat4_multiplication(roll_mat4, pitch_yaw_mat4, model_mat4);

	mat4_t view_mat4 = {
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, -4 },
		{ 0, 0, 0, 1 },
	};

	mat4_t projection_mat4;
	set_perspective_projection_mat4(projection_mat4, radians(60),
			(GLfloat)app->window_width / (GLfloat)app->window_height);

	mat4_t view_model_mat4;
	mat4_multiplication(view_mat4, model_mat4, view_model_mat4);

	mat4_t projection_view_model_mat4;
	mat4_multiplication(projection_mat4, view_model_mat4, projection_view_model_mat4);

	GLuint projection_view_model_uniform_location = glGetUniformLocation(app->program, "projection_view_model");
	assert(projection_view_model_uniform_location != (GLuint)-1);
	glUniformMatrix4fv(projection_view_model_uniform_location, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);

	debug(app, projection_view_model_mat4);

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

