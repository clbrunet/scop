#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/program.h"
#include "scop/events.h"

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

int initialization(app_t *app)
{
	app->window_width = 1280;
	app->window_height = 720;

	app->fov = 90;

	app->camera.position.x = 0;
	app->camera.position.y = 0;
	app->camera.position.z = 4;
	app->camera.rotation.x = 0;
	app->camera.rotation.y = 0;

	app->is_entering_free_flight = false;
	app->should_rotate = true;

	if (initialize_glfw(app) == -1) {
		return -1;
	}

	if (initialize_gl(app) == -1) {
		glfwTerminate();
		return -1;
	}

	int vertices_count = 32;
	GLfloat vertices[32 * 3] = {
		1.000000, 1.000000, -1.000000,
		1.000000, -1.000000, -1.000000,
		1.000000, 1.000000, 1.000000,
		1.000000, -1.000000, 1.000000,
		-1.000000, 1.000000, -1.000000,
		-1.000000, -1.000000, -1.000000,
		-1.000000, 1.000000, 1.000000,
		-1.000000, -1.000000, 1.000000,
		0.400619, 1.670410, 0.400619,
		-0.400619, 1.670410, 0.400619,
		0.400619, 1.670410, -0.400619,
		-0.400619, 1.670410, -0.400619,
		0.103853, -0.103853, -4.105815,
		-0.103853, -0.103853, -4.105815,
		0.103853, 0.103853, -4.105815,
		-0.103853, 0.103853, -4.105815,
		1.000000, 0.333333, -1.000000,
		1.000000, -0.333333, -1.000000,
		1.000000, 0.333333, 1.000000,
		1.000000, -0.333333, 1.000000,
		1.000000, 1.000000, -0.333333,
		1.000000, 1.000000, 0.333333,
		1.000000, -1.000000, 0.333333,
		1.000000, -1.000000, -0.333333,
		1.000000, 0.333333, 0.333333,
		1.000000, 0.333333, -0.333333,
		1.000000, -0.333333, 0.333333,
		1.000000, -0.333333, -0.333333,
		1.564986, 0.027293, 0.027293,
		1.564986, 0.027293, -0.027293,
		1.564986, -0.027293, 0.027293,
		1.564986, -0.027293, -0.027293,
	};
	app->triangle_count = 60;
	GLuint indices[60 * 3] = {
		8, 6, 2,
		7, 19, 18,
		14, 13, 15,
		18, 6, 7,
		14, 16, 17,
		4, 7, 6,
		8, 21, 20,
		9, 4, 6,
		20, 10, 8,
		3, 26, 19,
		13, 1, 5,
		23, 5, 1,
		15, 5, 4,
		14, 4, 0,
		23, 22, 7,
		30, 24, 26,
		18, 21, 2,
		19, 24, 18,
		27, 16, 25,
		25, 0, 20,
		31, 26, 27,
		24, 20, 21,
		23, 17, 27,
		22, 27, 26,
		30, 29, 28,
		17, 12, 14,
		28, 25, 24,
		29, 27, 25,
		11, 0, 4,
		7, 3, 19,
		14, 0, 16,
		8, 2, 21,
		22, 3, 7,
		9, 10, 11,
		8, 9, 6,
		14, 12, 13,
		18, 2, 6,
		4, 5, 7,
		9, 11, 4,
		20, 0, 10,
		3, 22, 26,
		13, 12, 1,
		23, 7, 5,
		15, 13, 5,
		14, 15, 4,
		30, 28, 24,
		18, 24, 21,
		19, 26, 24,
		27, 17, 16,
		25, 16, 0,
		31, 30, 26,
		24, 25, 20,
		23, 1, 17,
		22, 23, 27,
		30, 31, 29,
		17, 1, 12,
		28, 29, 25,
		29, 31, 27,
		11, 10, 0,
		9, 8, 10,
	};


	int vertices_dimension = 3;
	int indices_dimension = 3;

	glGenVertexArrays(1, &app->vertex_array);
	glGenBuffers(1, &app->vertex_buffer);
	glGenBuffers(1, &app->element_buffer);

	glBindVertexArray(app->vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, app->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * vertices_dimension * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, app->triangle_count * indices_dimension * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, vertices_dimension, GL_FLOAT, GL_FALSE,
			vertices_dimension * sizeof(GLfloat), (const GLvoid *)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	return 0;
}
