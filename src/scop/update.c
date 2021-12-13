#include <math.h>
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
#include "scop/utils/math.h"

static void process_inputs_movements(app_t *app)
{
	if (glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		return;
	}

	vec3_t movement = {};

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
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(app->camera.rotation.x));
	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(app->camera.rotation.y));

	mat4_t rotation_mat4;
	mat4_multiplication(yaw_mat4, pitch_mat4, rotation_mat4);

	vec4_t movement_vec4 = mat4_vec4_multiplication(rotation_mat4, vec3_to_vec4(&movement));
	movement = vec4_to_vec3(&movement_vec4);

	if (glfwGetKey(app->window, GLFW_KEY_Q) == GLFW_PRESS) {
		movement.y--;
	}
	if (glfwGetKey(app->window, GLFW_KEY_E) == GLFW_PRESS) {
		movement.y++;
	}
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	GLfloat speed;
	if (glfwGetKey(app->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 20;
	} else if (glfwGetKey(app->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		speed = 1;
	} else {
		speed = 8;
	}
	vec3_set_magnitude(&movement, speed * app->time.delta);
	app->camera.position = vec3_addition(&app->camera.position, &movement);
}

static void process_inputs(app_t *app)
{
	process_inputs_movements(app);
}

static void set_model_mat4(app_t *app, mat4_t model_mat4)
{
	if (app->should_model_rotate == false) {
		set_identity_mat4(model_mat4);
		return;
	}
	set_yaw_mat4(model_mat4, radians(app->time.current * 30));
}

static void set_view_mat4(app_t *app, mat4_t view_mat4)
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

static void set_projection_view_model_mat4(app_t *app, mat4_t projection_view_model_mat4)
{
	mat4_t model_mat4;
	set_model_mat4(app, model_mat4);

	mat4_t view_mat4;
	set_view_mat4(app, view_mat4);

	mat4_t projection_mat4;
	GLfloat aspect_ratio = (GLfloat)app->window_width / (GLfloat)app->window_height;
	if (app->should_use_orthographic == true) {
		set_orthographic_projection_mat4(projection_mat4,
				app->model_cubic_bounding_box.min * aspect_ratio,
				app->model_cubic_bounding_box.max * aspect_ratio,
				app->model_cubic_bounding_box.min,
				app->model_cubic_bounding_box.max, 0.1, 1000);
	} else {
		set_perspective_projection_mat4(projection_mat4, radians(app->fov),
				aspect_ratio, 0.1, 1000);
	}

	mat4_t view_model_mat4;
	mat4_multiplication(view_mat4, model_mat4, view_model_mat4);

	mat4_multiplication(projection_mat4, view_model_mat4, projection_view_model_mat4);
}

void update(app_t *app)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	assert(glGetError() == GL_NO_ERROR);

	process_inputs(app);

	mat4_t projection_view_model_mat4;
	set_projection_view_model_mat4(app, projection_view_model_mat4);

	glUniformMatrix4fv(app->uniforms.projection_view_model, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);
	assert(glGetError() == GL_NO_ERROR);

	glDrawArrays(GL_TRIANGLES, 0, app->triangle_count * 3);
	assert(glGetError() == GL_NO_ERROR);
}
