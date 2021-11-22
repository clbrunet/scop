#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	app_t *app = (app_t *)glfwGetWindowUserPointer(window);
	app->window_width = width;
	app->window_height = height;
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
	app_t *app = (app_t *)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		} else if (key == GLFW_KEY_L) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (key == GLFW_KEY_F) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (key == GLFW_KEY_P) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
	}
	if (key == GLFW_KEY_W) {
		app->camera_position.z -= 10 * app->delta_time;
	} else if (key == GLFW_KEY_S) {
		app->camera_position.z += 10 * app->delta_time;
	} else if (key == GLFW_KEY_A) {
		app->camera_position.x -= 10 * app->delta_time;
	} else if (key == GLFW_KEY_D) {
		app->camera_position.x += 10 * app->delta_time;
	}
}

static void print_mouse_button(int button, int action, int mods)
{
	return;
	printf("\n");
	printf("button: %i\n", button);
	printf("action: %i\n", action);
	printf("mods: %i\n", mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	(void)window;
	print_mouse_button(button, action, mods);
}

static void print_scroll(double xoffset, double yoffset)
{
	return;
	printf("\n");
	printf("xoffset: %f\n", xoffset);
	printf("yoffset: %f\n", yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	print_scroll(xoffset, yoffset);

	app_t *app = (app_t *)glfwGetWindowUserPointer(window);
	app->fov -= yoffset * 90 * app->delta_time;
	if (app->fov < 1) {
		app->fov = 1;
	} else if (app->fov > 179) {
		app->fov = 179;
	}
}
