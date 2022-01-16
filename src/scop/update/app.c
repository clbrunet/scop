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
#include "scop/update/utils.h"

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

static int draw_normals(app_t *app)
{
	glUseProgram(app->opengl.normals_program.id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glBindVertexArray(app->opengl.model_vao.id);
	assert(glGetError() == GL_NO_ERROR);

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

static int draw_model_lighting(app_t *app)
{
	glUseProgram(app->opengl.model_lighting_program.id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glBindVertexArray(app->opengl.model_vao.id);
	assert(glGetError() == GL_NO_ERROR);

	mat4_t view_mat4;
	set_view_mat4(app, view_mat4);
	mat4_t view_model_mat4;
	set_view_model_mat4(app, view_model_mat4);
	mat4_t projection_view_model_mat4;
	set_projection_view_model_mat4(app, projection_view_model_mat4);

	vec4_t light_position = mat4_vec4_multiplication(view_mat4,
			vec3_to_vec4(&app->light.position));

	glUniformMatrix4fv(app->opengl.model_lighting_program.view_model, 1, GL_TRUE,
			(const GLfloat *)view_model_mat4);
	assert(glGetError() == GL_NO_ERROR);
	glUniformMatrix4fv(app->opengl.model_lighting_program.projection_view_model, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);
	assert(glGetError() == GL_NO_ERROR);
	glUniform3fv(app->opengl.model_lighting_program.material_diffuse,
			1, app->model_info.color.array);
	assert(glGetError() == GL_NO_ERROR);
	glUniform3fv(app->opengl.model_lighting_program.light_position, 1, light_position.array);
	assert(glGetError() == GL_NO_ERROR);
	glUniform1f(app->opengl.model_lighting_program.texture_portion, app->texture_portion);
	assert(glGetError() == GL_NO_ERROR);

	glDrawArrays(GL_TRIANGLES, 0, app->model_info.triangles_count * 3);
	assert(glGetError() == GL_NO_ERROR);
	return 0;
}

static int draw_light(app_t *app)
{
	glUseProgram(app->opengl.light_program.id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glBindVertexArray(app->opengl.light_vao.id);
	assert(glGetError() == GL_NO_ERROR);

	mat4_t projection_view_mat4;
	set_projection_view_mat4(app, projection_view_mat4);

	glUniformMatrix4fv(app->opengl.light_program.projection_view, 1, GL_TRUE,
			(const GLfloat *)projection_view_mat4);
	assert(glGetError() == GL_NO_ERROR);

	glDrawArrays(GL_POINTS, 0, 1);
	assert(glGetError() == GL_NO_ERROR);
	return 0;
}

static int draw_model(app_t *app)
{
	glUseProgram(app->opengl.model_program.id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glBindVertexArray(app->opengl.model_vao.id);
	assert(glGetError() == GL_NO_ERROR);

	mat4_t projection_view_model_mat4;
	set_projection_view_model_mat4(app, projection_view_model_mat4);

	glUniformMatrix4fv(app->opengl.model_program.projection_view_model, 1, GL_TRUE,
			(const GLfloat *)projection_view_model_mat4);
	assert(glGetError() == GL_NO_ERROR);
	glUniform4fv(app->opengl.model_program.model_color,
			1, vec3_to_vec4(&app->model_info.color).array);
	assert(glGetError() == GL_NO_ERROR);
	glUniform1f(app->opengl.model_program.texture_portion, app->texture_portion);
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
	if (app->should_use_lighting) {
		if (draw_model_lighting(app) == -1
			|| draw_light(app) == -1) {
			return -1;
		}
	} else {
		if (draw_model(app) == -1) {
			return -1;
		}
	}
	return 0;
}
