#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/initialization.h"
#include "scop/app.h"
#include "scop/create_program.h"
#include "scop/events.h"
#include "scop/load_obj.h"
#include "scop/load_tga.h"
#include "scop/destruction.h"
#include "scop/vectors/vec3.h"
#include "scop/utils/math.h"

static void initialize_variables(app_t *app)
{
	app->window.width = 1280;
	app->window.height = 720;

	app->fov = 90;
	app->should_use_orthographic = false;

	app->camera.position.x = 0;
	app->camera.position.y = 0;
	app->camera.position.z = 10;
	app->camera.rotation.x = 0;
	app->camera.rotation.y = 0;

	app->is_entering_free_flight = false;

	app->model_info.yaw = 0;
	app->model_info.should_rotate = true;
	app->model_info.position = (vec3_t){ .x = 0, .y = 0, .z = 0 };

	app->texture_animation_phase = TO_COLOR;
	app->texture_portion = 0;

	app->selected_axis = X;
}

static int initialize_glfw_create_window(app_t *app)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
#endif

	app->window.ptr = glfwCreateWindow(app->window.width, app->window.height, "SCOP", NULL, NULL);
	if (app->window.ptr == NULL) {
		fprintf(stderr, "Window or OpenGL context creation failed failed\n");
		const char *description;
		glfwGetError(&description);
		if (description) {
			fprintf(stderr, "Error: %s\n", description);
		}
		glfwTerminate();
		return -1;
	}

	glfwSetWindowUserPointer(app->window.ptr, app);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	glfwMakeContextCurrent(app->window.ptr);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	glfwSwapInterval(1);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	return 0;
}

static void initialize_glfw_callbacks(app_t *app)
{
	glfwSetFramebufferSizeCallback(app->window.ptr, &framebuffer_size_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetKeyCallback(app->window.ptr, &key_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetMouseButtonCallback(app->window.ptr, &mouse_button_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetScrollCallback(app->window.ptr, &scroll_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetCursorPosCallback(app->window.ptr, &cursor_pos_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
}

static int initialize_glfw(app_t *app)
{
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "GLFW initialization failed\n");
		const char *description;
		glfwGetError(&description);
		if (description) {
			fprintf(stderr, "Error: %s\n", description);
		}
		return -1;
	}

	if (initialize_glfw_create_window(app) == -1) {
		return -1;
	}

	initialize_glfw_callbacks(app);
	return 0;
}

static int initialize_gl(app_t *app)
{
	if (gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress) == 0) {
		fprintf(stderr, "Glad initialization failed\n");
		return -1;
	}

	const GLubyte *version = glGetString(GL_VERSION);
	assert(version);
	printf("OpenGL version : %s\n", version);

	glViewport(0, 0, app->window.width, app->window.height);
	assert(glGetError() == GL_NO_ERROR);
	glEnable(GL_DEPTH_TEST);
	assert(glGetError() == GL_NO_ERROR);
	glPointSize(2);
	assert(glGetError() == GL_NO_ERROR);

	app->opengl.program = create_program("./shaders/main.vert", "./shaders/main.frag");
	if (app->opengl.program == 0) {
		glfwTerminate();
		return -1;
	}
	glUseProgram(app->opengl.program);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		glDeleteProgram(app->opengl.program);
		return -1;
	}
	assert(error == GL_NO_ERROR);

	app->opengl.uniforms.projection_view_model = glGetUniformLocation(app->opengl.program, "projection_view_model");
	assert(app->opengl.uniforms.projection_view_model != -1);
	app->opengl.uniforms.texture_portion = glGetUniformLocation(app->opengl.program, "texture_portion");
	assert(app->opengl.uniforms.texture_portion != -1);
	return 0;
}

static vec2_t get_texture_coordinates(const vec3_t *position, const vec3_t *normal,
		const bounding_box_t *model_bounding_box, GLfloat texture_aspect_ratio)
{
	vec2_t texture_coordinates = {};

	vec3_t abs_normal = {
		.x = fabsf(normal->x),
		.y = fabsf(normal->y),
		.z = fabsf(normal->z),
	};
	if (abs_normal.x > abs_normal.y && abs_normal.x > abs_normal.z) {
		texture_coordinates.u = (-position->z - model_bounding_box->z.min) / texture_aspect_ratio;
		texture_coordinates.v = position->y - model_bounding_box->y.min;
	} else if (abs_normal.y > abs_normal.x && abs_normal.y > abs_normal.z) {
		texture_coordinates.u = (position->x - model_bounding_box->x.min) / texture_aspect_ratio;
		texture_coordinates.v = -position->z - model_bounding_box->z.min;
	} else {
		texture_coordinates.u = (position->x - model_bounding_box->x.min) / texture_aspect_ratio;
		texture_coordinates.v = position->y - model_bounding_box->y.min;
	}
	return texture_coordinates;
}

static int initialize_array_buffer_data(array_buffer_data_t *array_buffer_data,
		const model_t *model, GLfloat texture_aspect_ratio)
{
	array_buffer_data->vertices_count = model->triangles_count * 3;
	array_buffer_data->vertices = malloc(array_buffer_data->vertices_count * sizeof(vertex_t));
	if (array_buffer_data->vertices == NULL) {
		err(1, "malloc");
		return -1;
	}

	GLfloat whiteness = 1;
	GLfloat whiteness_shift = -0.04;
	vertex_t *vertex_it = array_buffer_data->vertices;
	triangle_t *triangle_it = model->triangles;
	for (GLuint i = 0; i < model->triangles_count; i++) {
		vec3_t color = vec3(whiteness, whiteness, whiteness);
		vec3_t normal = get_triangle_normal(&model->vertices_position[(*triangle_it)[0]],
				&model->vertices_position[(*triangle_it)[1]],
				&model->vertices_position[(*triangle_it)[2]]);
		vertex_it[0].position = model->vertices_position[(*triangle_it)[0]];
		vertex_it[0].color = color;
		vertex_it[0].texture_coordinates = get_texture_coordinates(
				&vertex_it[0].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[1].position = model->vertices_position[(*triangle_it)[1]];
		vertex_it[1].color = color;
		vertex_it[1].texture_coordinates = get_texture_coordinates(
				&vertex_it[1].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[2].position = model->vertices_position[(*triangle_it)[2]];
		vertex_it[2].color = color;
		vertex_it[2].texture_coordinates = get_texture_coordinates(
				&vertex_it[2].position, &normal, &model->bounding_box, texture_aspect_ratio);

		vertex_it += 3;
		triangle_it++;
		whiteness += whiteness_shift;
		if (whiteness < 0.3 || 1 < whiteness) {
			whiteness_shift = -whiteness_shift;
			whiteness += whiteness_shift;
		}
	}
	return 0;
}

static int initialize_array_buffer(const app_t *app, const array_buffer_data_t *array_buffer_data)
{
	glBindBuffer(GL_ARRAY_BUFFER, app->opengl.vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, array_buffer_data->vertices_count * sizeof(vertex_t),
			array_buffer_data->vertices, GL_STATIC_DRAW);
	GLenum error = glGetError();
	if (error == GL_OUT_OF_MEMORY) {
		fprintf(stderr, "glBufferData out of memory error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);
	return 0;
}

static void initialize_vertex_array()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, position));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(0);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, color));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, texture_coordinates));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(2);
	assert(glGetError() == GL_NO_ERROR);
}

static int initialize_texture_map(const app_t *app, const texture_t *texture)
{
	glActiveTexture(GL_TEXTURE0);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, app->opengl.texture_map);
	assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	assert(glGetError() == GL_NO_ERROR);
	if (texture->channel_count == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_BGR,
				GL_UNSIGNED_BYTE, texture->data);
	} else if (texture->channel_count == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_BGRA,
				GL_UNSIGNED_BYTE, texture->data);
	} else {
		fprintf(stderr, "Support only 3 or 4 texture channel count.\n");
		return -1;
	}
	GLenum error = glGetError();
	if (error == GL_INVALID_VALUE) {
		if ((u_int)texture->width > GL_MAX_TEXTURE_SIZE
			|| (u_int)texture->height > GL_MAX_TEXTURE_SIZE) {
			fprintf(stderr, "Texture too large.\n");
			return -1;
		}
		assert(false);
	}
	assert(error == GL_NO_ERROR);
	glGenerateMipmap(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);
	GLint sampler_uniform = glGetUniformLocation(app->opengl.program, "sampler");
	assert(sampler_uniform != -1);
	glUniform1i(sampler_uniform, 0);
	assert(glGetError() == GL_NO_ERROR);
	return 0;
}

int initialization(app_t *app, const char *object_path, const char *texture_path)
{
	initialize_variables(app);
	if (initialize_glfw(app) == -1) {
		return -1;
	}
	if (initialize_gl(app) == -1) {
		glfwTerminate();
		return -1;
	}

	model_t model;
	if (load_obj(&model, object_path) == -1) {
		glDeleteProgram(app->opengl.program);
		glfwTerminate();
		return -1;
	}
	app->model_info.triangles_count = model.triangles_count;
	app->model_info.bounding_box = model.bounding_box;
	texture_t texture;
	texture.data = load_tga(texture_path, &texture.width, &texture.height,
			&texture.channel_count);
	if (texture.data == NULL) {
		free(model.vertices_position);
		free(model.triangles);
		glDeleteProgram(app->opengl.program);
		glfwTerminate();
		return -1;
	}
	array_buffer_data_t array_buffer_data;
	if (initialize_array_buffer_data(&array_buffer_data, &model,
			(GLfloat)texture.width / (GLfloat)texture.height) == -1) {
		free(model.vertices_position);
		free(model.triangles);
		free(texture.data);
		glDeleteProgram(app->opengl.program);
		glfwTerminate();
		return -1;
	}
	free(model.vertices_position);
	free(model.triangles);

	glGenVertexArrays(1, &app->opengl.vertex_array);
	assert(glGetError() == GL_NO_ERROR);
	glGenBuffers(1, &app->opengl.vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glGenTextures(1, &app->opengl.texture_map);
	assert(glGetError() == GL_NO_ERROR);

	glBindVertexArray(app->opengl.vertex_array);
	assert(glGetError() == GL_NO_ERROR);

	if (initialize_array_buffer(app, &array_buffer_data) == -1) {
		free(array_buffer_data.vertices);
		free(texture.data);
		destruction(app);
		return -1;
	}
	free(array_buffer_data.vertices);
	initialize_vertex_array();
	if (initialize_texture_map(app, &texture) == -1) {
		free(texture.data);
		destruction(app);
		return -1;
	}
	free(texture.data);
	return 0;
}
