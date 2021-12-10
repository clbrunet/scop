#include "scop/vectors/vec3.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

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
	assert(app->uniforms.projection_view_model != (GLuint)-1);
	return 0;
}

static void initialize_model_cubic_bounding_box(app_t *app, const model_t *model)
{
	GLfloat min = 0;
	GLfloat max = 0;

	vec3_t *vertices_it = model->vertices;
	for (GLuint i = 0; i < model->vertex_count; i++) {
		if (vertices_it->x < min) {
			min = vertices_it->x;
		}
		if (vertices_it->y < min) {
			min = vertices_it->y;
		}
		if (vertices_it->z < min) {
			min = vertices_it->z;
		}

		if (max < vertices_it->x) {
			max = vertices_it->x;
		}
		if (max < vertices_it->y) {
			max = vertices_it->y;
		}
		if (max < vertices_it->z) {
			max = vertices_it->z;
		}
		vertices_it++;
	}

	app->model_cubic_bounding_box.min = min;
	app->model_cubic_bounding_box.max = max;
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
	initialize_model_cubic_bounding_box(app, &model);
	glGenVertexArrays(1, &app->vertex_array);
	assert(glGetError() == GL_NO_ERROR);
	glGenBuffers(1, &app->vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glGenBuffers(1, &app->element_buffer);
	assert(glGetError() == GL_NO_ERROR);

	glBindVertexArray(app->vertex_array);
	assert(glGetError() == GL_NO_ERROR);

	glBindBuffer(GL_ARRAY_BUFFER, app->vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, model.vertex_count * sizeof(vec3_t), model.vertices, GL_STATIC_DRAW);
	GLenum error = glGetError();
	if (error == GL_OUT_OF_MEMORY) {
		fprintf(stderr, "glBufferData out of memory error\n");
		destruction(app);
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->element_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.triangle_count * sizeof(triangle_t), model.triangles, GL_STATIC_DRAW);
	error = glGetError();
	if (error == GL_OUT_OF_MEMORY) {
		fprintf(stderr, "glBufferData out of memory error\n");
		destruction(app);
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3_t), (const GLvoid *)0);
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(0);
	assert(glGetError() == GL_NO_ERROR);

	free(model.vertices);
	free(model.triangles);

	// @todo textures
	// @todo errors management
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
	// 		(3 + 2) * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
	// glEnableVertexAttribArray(1);

	// int width;
	// int height;
	// int channel_count;
	// unsigned char *data = tga_load("./textures/blue.tga", &width, &height, &channel_count);
	// if (data == NULL) {
	// 	glfwTerminate();
	// 	return 1;
	// }
	// unsigned int texture;
	// glGenTextures(1, &texture);
	// glBindTexture(GL_TEXTURE_2D, texture);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	// free(data);
	return 0;
}
