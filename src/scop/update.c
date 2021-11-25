#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/vectors/vec2_double.h"
#include "scop/vectors/vec3.h"
#include "scop/matrices/mat4.h"
#include "scop/vectors/vec4.h"
#include "scop/utils.h"

void process_inputs_movements(app_t *app)
{
	if (glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		return;
	}
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
	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(app->camera.rotation.x));
	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(app->camera.rotation.y));

	mat4_t rotation_mat4;
	mat4_multiplication(yaw_mat4, pitch_mat4, rotation_mat4);

	vec4_t movement_vec4 = mat4_vec4_multiplication(rotation_mat4, vec3_to_vec4(&movement));
	movement = vec4_to_vec3(&movement_vec4);

	vec3_set_magnitude(&movement, 10 * app->time.delta);
	app->camera.position = vec3_addition(&app->camera.position, &movement);
}

void process_inputs(app_t *app)
{
	process_inputs_movements(app);
}

void set_model_mat4(app_t *app, mat4_t model_mat4)
{
	if (app->should_rotate == false) {
		set_identity_mat4(model_mat4);
		return;
	}
	set_yaw_mat4(model_mat4, radians(app->time.current * 30));
}

void set_view_mat4(app_t *app, mat4_t view_mat4)
{
	mat4_t translation_mat4 = {
		{ 1, 0, 0, -app->camera.position.x },
		{ 0, 1, 0, -app->camera.position.y },
		{ 0, 0, 1, -app->camera.position.z },
		{ 0, 0, 0, 1 },
	};

	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(-app->camera.rotation.y));
	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(-app->camera.rotation.x));

	mat4_t rotation_mat4;
	mat4_multiplication(pitch_mat4, yaw_mat4, rotation_mat4);

	mat4_multiplication(rotation_mat4, translation_mat4, view_mat4);
}

void set_projection_view_model_mat4(app_t *app, mat4_t projection_view_model_mat4)
{
	mat4_t model_mat4;
	set_model_mat4(app, model_mat4);

	mat4_t view_mat4;
	set_view_mat4(app, view_mat4);

	mat4_t projection_mat4;
	set_perspective_projection_mat4(projection_mat4, radians(app->fov),
			(GLfloat)app->window_width / (GLfloat)app->window_height, 0.1, 100);

	mat4_t view_model_mat4;
	mat4_multiplication(view_mat4, model_mat4, view_model_mat4);

	mat4_multiplication(projection_mat4, view_model_mat4, projection_view_model_mat4);
}

void update(app_t *app)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	process_inputs(app);

	mat4_t projection_view_model_mat4;
	set_projection_view_model_mat4(app, projection_view_model_mat4);

	glUniformMatrix4fv(app->uniforms.projection_view_model, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);

	GLfloat white = 1;
	for (GLsizei i = 0; i < app->triangle_count; i++) {
		glUniform4f(app->uniforms.color, white, white, white, 1);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid *)(i * 3 * sizeof(GLuint)));

		white -= 0.015;
		if (white < 0.2) {
			white = 1;
		}
	}
}
