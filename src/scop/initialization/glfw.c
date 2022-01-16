#include <stdio.h>
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "scop/app.h"
#include "scop/events.h"

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

int initialize_glfw(app_t *app)
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

