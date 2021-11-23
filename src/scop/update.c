#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/vec3.h"
#include "scop/mat4.h"
#include "scop/vec4.h"
#include "scop/utils.h"

void debug(app_t *app, mat4_t projection, mat4_t final)
{
	(void)app;
	(void)projection;
	(void)final;
	static int is_first = 1;
	if (is_first) {
		is_first = 0;
	}
}

void process_inputs_movements(app_t *app)
{
	vec3_t movement = { 0 };
	if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_PRESS) {
		movement.z--;
	}
	if (glfwGetKey(app->window, GLFW_KEY_S) == GLFW_PRESS) {
		movement.z++;
	}
	if (glfwGetKey(app->window, GLFW_KEY_A) == GLFW_PRESS) {
		movement.x--;
	}
	if (glfwGetKey(app->window, GLFW_KEY_D) == GLFW_PRESS) {
		movement.x++;
	}
	if (glfwGetKey(app->window, GLFW_KEY_E) == GLFW_PRESS) {
		movement.y--;
	}
	if (glfwGetKey(app->window, GLFW_KEY_Q) == GLFW_PRESS) {
		movement.y++;
	}
	vec3_set_magnitude(&movement, 10 * app->delta_time);
	app->camera_position = vec3_addition(&app->camera_position, &movement);
}

void process_inputs(app_t *app)
{
	process_inputs_movements(app);
}

void update(app_t *app)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	process_inputs(app);

	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(app->current_time * 40 * 1));
	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(0));
	mat4_t roll_mat4;
	set_roll_mat4(roll_mat4, radians(app->current_time * 40 * 0));
	mat4_t pitch_yaw_mat4;
	mat4_multiplication(pitch_mat4, yaw_mat4, pitch_yaw_mat4);
	mat4_t model_mat4;
	set_identity_mat4(model_mat4);
	mat4_multiplication(roll_mat4, pitch_yaw_mat4, model_mat4);

	mat4_t view_mat4 = {
		{ 1, 0, 0, -app->camera_position.x },
		{ 0, 1, 0, -app->camera_position.y },
		{ 0, 0, 1, -app->camera_position.z },
		{ 0, 0, 0, 1 },
	};

	mat4_t projection_mat4;
	set_perspective_projection_mat4(projection_mat4, radians(app->fov),
			(GLfloat)app->window_width / (GLfloat)app->window_height, 0.1, 100);

	mat4_t view_model_mat4;
	mat4_multiplication(view_mat4, model_mat4, view_model_mat4);

	mat4_t projection_view_model_mat4;
	mat4_multiplication(projection_mat4, view_model_mat4, projection_view_model_mat4);

	GLuint projection_view_model_uniform_location = glGetUniformLocation(app->program, "projection_view_model");
	assert(projection_view_model_uniform_location != (GLuint)-1);
	glUniformMatrix4fv(projection_view_model_uniform_location, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);

	debug(app, projection_mat4, projection_view_model_mat4);

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

