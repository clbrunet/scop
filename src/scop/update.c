#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/update.h"
#include "scop/app.h"
#include "scop/vectors/vec2_double.h"
#include "scop/vectors/vec3.h"
#include "scop/matrices/mat4.h"
#include "scop/vectors/vec4.h"
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

static void process_inputs(app_t *app)
{
	process_inputs_movements(app);
	process_inputs_move_model(app);
}

static void process_animations(app_t *app)
{
	if (app->texture_animation_phase == TO_COLOR) {
		app->texture_portion -= app->time.delta * 0.5;
		if (app->texture_portion < 0) {
			app->texture_portion = 0;
		}
	} else {
		app->texture_portion += app->time.delta * 0.5;
		if (app->texture_portion > 1) {
			app->texture_portion = 1;
		}
	}

	if (app->model_info.should_rotate == true) {
		app->model_info.yaw += app->time.delta * 30;
		if (app->model_info.yaw > 360) {
			app->model_info.yaw -= 360;
		}
	}
}

static void set_model_mat4(app_t *app, mat4_t model_mat4)
{
	mat4_t translation_mat4 = {
		{ 1, 0, 0, app->model_info.position.x },
		{ 0, 1, 0, app->model_info.position.y },
		{ 0, 0, 1, app->model_info.position.z },
		{ 0, 0, 0, 1 },
	};

	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(app->model_info.yaw));

	mat4_multiplication(translation_mat4, yaw_mat4, model_mat4);
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

static void set_projection_mat4(app_t *app, mat4_t projection_mat4)
{
	GLfloat aspect_ratio = (GLfloat)app->window.width / (GLfloat)app->window.height;
	if (app->should_use_orthographic == true) {
		set_orthographic_projection_mat4(projection_mat4,
				-app->model_info.bounding_box.max_distance * aspect_ratio * 1.1,
				app->model_info.bounding_box.max_distance * aspect_ratio * 1.1,
				-app->model_info.bounding_box.max_distance * 1.1,
				app->model_info.bounding_box.max_distance * 1.1, NEAR_PLANE, FAR_PLANE);
	} else {
		set_perspective_projection_mat4(projection_mat4, radians(app->fov),
				aspect_ratio, NEAR_PLANE, FAR_PLANE);
	}
}

static void set_view_model_mat4(app_t *app, mat4_t view_model_mat4)
{
	mat4_t model_mat4;
	set_model_mat4(app, model_mat4);

	mat4_t view_mat4;
	set_view_mat4(app, view_mat4);

	mat4_multiplication(view_mat4, model_mat4, view_model_mat4);
}

static void set_projection_view_model_mat4(app_t *app, mat4_t projection_view_model_mat4)
{
	mat4_t view_model_mat4;
	set_view_model_mat4(app, view_model_mat4);

	mat4_t projection_mat4;
	set_projection_mat4(app, projection_mat4);

	mat4_multiplication(projection_mat4, view_model_mat4, projection_view_model_mat4);
}

static int draw_normals(app_t *app)
{
	glUseProgram(app->opengl.normals_program.id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	mat4_t view_model_mat4;
	set_view_model_mat4(app, view_model_mat4);
	mat4_t projection_mat4;
	set_projection_mat4(app, projection_mat4);

	glUniformMatrix4fv(app->opengl.normals_program.view_model, 1, GL_TRUE,
			(const GLfloat *)view_model_mat4);
	assert(glGetError() == GL_NO_ERROR);
	glUniformMatrix4fv(app->opengl.normals_program.projection, 1, GL_TRUE,
			(const GLfloat *)projection_mat4);
	assert(glGetError() == GL_NO_ERROR);

	glDrawArrays(GL_TRIANGLES, 0, app->model_info.triangles_count * 3);
	assert(glGetError() == GL_NO_ERROR);
	return 0;
}

static int draw_triangles(app_t *app)
{
	mat4_t projection_view_model_mat4;
	set_projection_view_model_mat4(app, projection_view_model_mat4);

	glUseProgram(app->opengl.triangles_program.id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glUniformMatrix4fv(app->opengl.triangles_program.projection_view_model, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);
	assert(glGetError() == GL_NO_ERROR);
	glUniform1f(app->opengl.triangles_program.texture_portion, app->texture_portion);
	assert(glGetError() == GL_NO_ERROR);

	glDrawArrays(GL_TRIANGLES, 0, app->model_info.triangles_count * 3);
	assert(glGetError() == GL_NO_ERROR);
	return 0;
}

int update(app_t *app)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	assert(glGetError() == GL_NO_ERROR);

	process_inputs(app);
	process_animations(app);

	if (app->should_display_normals) {
		if (draw_normals(app) == -1) {
			return -1;
		}
	}
	if (draw_triangles(app) == -1) {
		return -1;
	}
	return 0;
}
