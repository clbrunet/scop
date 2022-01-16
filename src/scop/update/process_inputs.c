#include <assert.h>

#include "scop/app.h"
#include "scop/vectors/vec4.h"
#include "scop/matrices/mat4.h"
#include "scop/utils/math.h"

static void process_inputs_movements(app_t *app)
{
	if (glfwGetMouseButton(app->window.ptr, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		return;
	}

	vec3_t movement = {};

	if (glfwGetKey(app->window.ptr, GLFW_KEY_W) == GLFW_PRESS) {
		movement.z--;
	}
	if (glfwGetKey(app->window.ptr, GLFW_KEY_S) == GLFW_PRESS) {
		movement.z++;
	}
	if (glfwGetKey(app->window.ptr, GLFW_KEY_A) == GLFW_PRESS) {
		movement.x--;
	}
	if (glfwGetKey(app->window.ptr, GLFW_KEY_D) == GLFW_PRESS) {
		movement.x++;
	}
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(app->camera.rotation.x));
	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(app->camera.rotation.y));

	mat4_t rotation_mat4;
	mat4_multiplication(yaw_mat4, pitch_mat4, rotation_mat4);

	vec4_t movement_vec4 = mat4_vec4_multiplication(rotation_mat4, vec3_to_vec4(&movement));
	movement = vec4_to_vec3(&movement_vec4);

	if (glfwGetKey(app->window.ptr, GLFW_KEY_Q) == GLFW_PRESS) {
		movement.y--;
	}
	if (glfwGetKey(app->window.ptr, GLFW_KEY_E) == GLFW_PRESS) {
		movement.y++;
	}
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	GLfloat speed = app->camera.speed;
	if (glfwGetKey(app->window.ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed *= 2;
	}
	if (glfwGetKey(app->window.ptr, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		speed /= 2;
	}
	vec3_set_magnitude(&movement, speed * app->time.delta);
	app->camera.position = vec3_addition(&app->camera.position, &movement);
}

static void process_inputs_move_model(app_t *app)
{
	float movement = 0;
	if (glfwGetKey(app->window.ptr, GLFW_KEY_MINUS) == GLFW_PRESS) {
		movement--;
	}
	if (glfwGetKey(app->window.ptr, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		movement++;
	}
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	GLfloat speed;
	if (glfwGetKey(app->window.ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 3;
	} else if (glfwGetKey(app->window.ptr, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		speed = 0.5;
	} else {
		speed = 1;
	}
	movement *= speed * app->time.delta;
	switch (app->selected_axis) {
		case X:
			app->model_info.position.x += movement;
			break;
		case Y:
			app->model_info.position.y += movement;
			break;
		case Z:
			app->model_info.position.z += movement;
			break;
	}
}

void process_inputs(app_t *app)
{
	process_inputs_movements(app);
	process_inputs_move_model(app);
}
