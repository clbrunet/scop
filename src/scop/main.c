#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h> 

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// printf("\n");
	// printf("key: %i\n", key);
	// printf("scancode: %i\n", scancode);
	// printf("action: %i\n", action);
	// printf("mods: %i\n", mods);
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

GLFWwindow *initialize_glfw()
{
	GLFWwindow *window = NULL;
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "GLFW initialization failed\n");
		return NULL;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "SCOP", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Window or OpenGL context creation failed failed\n");
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);
	glfwSetKeyCallback(window, &key_callback);
	return window;
}

int initialize_gl()
{
	if (gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress) == 0) {
		fprintf(stderr, "Glad initialization failed\n");
		return -1;
	}

	glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);
	return 0;
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	GLFWwindow *window = initialize_glfw();
	if (window == NULL) {
		return 1;
	}
	if (initialize_gl() == -1) {
		return 1;
	}

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
