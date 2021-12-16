#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>

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
#include "scop/utils/math.h"

static void initialize_variables(app_t *app)
{
	app->window_width = 1280;
	app->window_height = 720;

	app->fov = 90;

	app->camera.position.x = 0;
	app->camera.position.y = 0;
	app->camera.position.z = 10;

	app->camera.rotation.x = 0;
	app->camera.rotation.y = 0;

	app->is_entering_free_flight = false;
	app->should_model_rotate = true;
	app->should_use_orthographic = false;
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

	app->window = glfwCreateWindow(app->window_width, app->window_height, "SCOP", NULL, NULL);
	if (app->window == NULL) {
		fprintf(stderr, "Window or OpenGL context creation failed failed\n");
		const char *description;
		glfwGetError(&description);
		if (description) {
			fprintf(stderr, "Error: %s\n", description);
		}
		glfwTerminate();
		return -1;
	}

	glfwSetWindowUserPointer(app->window, app);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	glfwMakeContextCurrent(app->window);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);

	glfwSwapInterval(1);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	return 0;
}

static void initialize_glfw_callbacks(app_t *app)
{
	glfwSetFramebufferSizeCallback(app->window, &framebuffer_size_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetKeyCallback(app->window, &key_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetMouseButtonCallback(app->window, &mouse_button_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetScrollCallback(app->window, &scroll_callback);
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	glfwSetCursorPosCallback(app->window, &cursor_pos_callback);
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

	glViewport(0, 0, app->window_width, app->window_height);
	assert(glGetError() == GL_NO_ERROR);
	glEnable(GL_DEPTH_TEST);
	assert(glGetError() == GL_NO_ERROR);
	glPointSize(2);
	assert(glGetError() == GL_NO_ERROR);

	app->program = create_program("./shaders/main.vert", "./shaders/main.frag");
	if (app->program == 0) {
		glfwTerminate();
		return -1;
	}
	glUseProgram(app->program);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		glDeleteProgram(app->program);
		return -1;
	}
	assert(error == GL_NO_ERROR);

	app->uniforms.projection_view_model = glGetUniformLocation(app->program, "projection_view_model");
	assert(app->uniforms.projection_view_model != -1);
	return 0;
}

static int initialize_array_buffer_data(array_buffer_data_t *array_buffer_data,
		const model_t *model, GLfloat texture_aspect_ratio)
{
	array_buffer_data->vertex_count = model->triangle_count * 3;
	array_buffer_data->vertices = malloc(array_buffer_data->vertex_count * sizeof(vertex_t));
	if (array_buffer_data->vertices == NULL) {
		err(1, "malloc");
		return -1;
	}

	GLfloat whiteness = 1;
	GLfloat whiteness_shift = -0.04;
	vertex_t *vertex_it = array_buffer_data->vertices;
	triangle_t *triangle_it = model->triangles;
	for (GLuint i = 0; i < model->triangle_count; i++) {
		vec3_t color = vec3(whiteness, whiteness, whiteness);
		vec3_t normal = get_triangle_normal(&model->vertices[(*triangle_it)[0]],
				&model->vertices[(*triangle_it)[1]],
				&model->vertices[(*triangle_it)[2]]);
		vertex_it[0].position = model->vertices[(*triangle_it)[0]];
		vertex_it[0].color = color;
		vertex_it[0].texture_coordinates = get_texture_coordinates(
				&vertex_it[0].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[1].position = model->vertices[(*triangle_it)[1]];
		vertex_it[1].color = color;
		vertex_it[1].texture_coordinates = get_texture_coordinates(
				&vertex_it[1].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[2].position = model->vertices[(*triangle_it)[2]];
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

int initialization(app_t *app, const char *object_path)
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
		glDeleteProgram(app->program);
		glfwTerminate();
		return -1;
	}
	app->triangle_count = model.triangle_count;
	app->model_bounding_box = model.bounding_box;
	int texture_width;
	int texture_height;
	int texture_channel_count;
	u_char *texture_data = load_tga("./textures/uv_grid.tga",
			&texture_width, &texture_height, &texture_channel_count);
	if (texture_data == NULL) {
		destruction(app);
		return -1;
	}
	array_buffer_data_t array_buffer_data;
	if (initialize_array_buffer_data(&array_buffer_data, &model,
			(GLfloat)texture_width / (GLfloat)texture_height) == -1) {
		free(model.vertices);
		free(model.triangles);
		free(texture_data);
		glDeleteProgram(app->program);
		glfwTerminate();
		return -1;
	}
	free(model.vertices);
	free(model.triangles);

	glGenVertexArrays(1, &app->vertex_array);
	assert(glGetError() == GL_NO_ERROR);
	glGenBuffers(1, &app->vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glGenTextures(1, &app->texture_map);
	assert(glGetError() == GL_NO_ERROR);

	glBindVertexArray(app->vertex_array);
	assert(glGetError() == GL_NO_ERROR);

	glBindBuffer(GL_ARRAY_BUFFER, app->vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, array_buffer_data.vertex_count * sizeof(vertex_t),
			array_buffer_data.vertices, GL_STATIC_DRAW);
	GLenum error = glGetError();
	if (error == GL_OUT_OF_MEMORY) {
		fprintf(stderr, "glBufferData out of memory error\n");
		destruction(app);
		return -1;
	}
	assert(error == GL_NO_ERROR);
	free(array_buffer_data.vertices);

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

	glActiveTexture(GL_TEXTURE0);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, app->texture_map);
	assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	assert(glGetError() == GL_NO_ERROR);
	if (texture_channel_count == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_BGR,
				GL_UNSIGNED_BYTE, texture_data);
	} else if (texture_channel_count == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_BGRA,
				GL_UNSIGNED_BYTE, texture_data);
	} else {
		assert(false);
	}
	assert(glGetError() == GL_NO_ERROR);
	glGenerateMipmap(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);
	GLint sampler_uniform = glGetUniformLocation(app->program, "sampler");
	assert(sampler_uniform != -1);
	glUniform1i(sampler_uniform, 0);
	assert(glGetError() == GL_NO_ERROR);
	free(texture_data);
	return 0;
}
