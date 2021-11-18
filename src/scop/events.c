#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

static void print_key(int key, int scancode, int action, int mods)
{
	return;
	printf("\n");
	printf("key: %i\n", key);
	printf("scancode: %i\n", scancode);
	printf("action: %i\n", action);
	printf("mods: %i\n", mods);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	print_key(key, scancode, action, mods);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		else if (key == GLFW_KEY_L) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (key == GLFW_KEY_F) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (key == GLFW_KEY_P) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
	}

}

