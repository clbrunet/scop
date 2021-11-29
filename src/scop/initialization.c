#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/initialization.h"
#include "scop/app.h"
#include "scop/program.h"
#include "scop/events.h"
#include "scop/load_model.h"
#include "scop/tga.h"

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
	app->should_rotate = true;
}

static int initialize_glfw(app_t *app)
{
	app->window = NULL;
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "GLFW initialization failed\n");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	app->window = glfwCreateWindow(app->window_width, app->window_height, "SCOP", NULL, NULL);
	if (app->window == NULL) {
		fprintf(stderr, "Window or OpenGL context creation failed failed\n");
		glfwTerminate();
		return -1;
	}

	glfwSetWindowUserPointer(app->window, app);
	glfwMakeContextCurrent(app->window);

	glfwSetFramebufferSizeCallback(app->window, &framebuffer_size_callback);
	glfwSetKeyCallback(app->window, &key_callback);
	glfwSetMouseButtonCallback(app->window, &mouse_button_callback);
	glfwSetScrollCallback(app->window, &scroll_callback);
	glfwSetCursorPosCallback(app->window, &cursor_pos_callback);

	glfwSwapInterval(1);
	return 0;
}

static int initialize_gl(app_t *app)
{
	if (gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress) == 0) {
		fprintf(stderr, "Glad initialization failed\n");
		return -1;
	}

	printf("OpenGL version : %s\n", glGetString(GL_VERSION));
	glViewport(0, 0, app->window_width, app->window_height);
	glEnable(GL_DEPTH_TEST);

	app->program = create_program("./shaders/vertex.glsl", "./shaders/fragment.glsl");
	if (app->program == 0) {
		glfwTerminate();
		return -1;
	}
	glUseProgram(app->program);

	app->uniforms.projection_view_model = glGetUniformLocation(app->program, "projection_view_model");
	assert(app->uniforms.projection_view_model != (GLuint)-1);
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
	if (load_model(&model, object_path) == -1) {
		glfwTerminate();
		return -1;
	}
	app->triangle_count = model.triangle_count;
	glGenVertexArrays(1, &app->vertex_array);
	glGenBuffers(1, &app->vertex_buffer);
	glGenBuffers(1, &app->element_buffer);

	glBindVertexArray(app->vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, app->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, model.vertex_count * sizeof(vec3_t), model.vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.triangle_count * sizeof(triangle_t), model.triangles, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3_t), (const GLvoid *)0);
	glEnableVertexAttribArray(0);
	free(model.vertices);
	free(model.triangles);

	// @todo textures
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
