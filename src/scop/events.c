#include <stdio.h>
#include <stdbool.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/matrices/mat4.h"
#include "scop/vectors/vec3.h"
#include "scop/vectors/vec4.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	app_t *app = (app_t *)glfwGetWindowUserPointer(window);
	app->window.width = width;
	app->window.height = height;
	glViewport(0, 0, width, height);
}

static void debug_key(int key, int scancode, int action, int mods)
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
	debug_key(key, scancode, action, mods);
	app_t *app = (app_t *)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		if (key == GLFW_KEY_P) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		} else if (key == GLFW_KEY_L) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (key == GLFW_KEY_F) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (key == GLFW_KEY_R) {
			if (app->should_model_rotate == true) {
				app->should_model_rotate = false;
			} else {
				app->should_model_rotate = true;
			}
		}

		if (key == GLFW_KEY_O) {
			if (app->should_use_orthographic == false) {
				app->should_use_orthographic = true;
			} else {
				app->should_use_orthographic = false;
			}
		}

		if (key == GLFW_KEY_T) {
			if (app->texture_animation_phase == TO_COLOR) {
				app->texture_animation_phase = TO_TEXTURE;
			} else {
				app->texture_animation_phase = TO_COLOR;
			}
		}
	}
}

static void debug_mouse_button(int button, int action, int mods)
{
	return;
	printf("\n");
	printf("button: %i\n", button);
	printf("action: %i\n", action);
	printf("mods: %i\n", mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	debug_mouse_button(button, action, mods);
	app_t *app = (app_t *)glfwGetWindowUserPointer(window);

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			app->is_entering_free_flight = true;
		} else if (action == GLFW_RELEASE) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

static void debug_scroll(double xoffset, double yoffset)
{
	return;
	printf("\n");
	printf("xoffset: %f\n", xoffset);
	printf("yoffset: %f\n", yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	debug_scroll(xoffset, yoffset);

	app_t *app = (app_t *)glfwGetWindowUserPointer(window);
	app->fov -= yoffset * 3;
	if (app->fov < 60) {
		app->fov = 60;
	} else if (app->fov > 120) {
		app->fov = 120;
	}
}
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		return;
	}

	app_t *app = (app_t *)glfwGetWindowUserPointer(window);

	if (app->is_entering_free_flight) {
		app->cursor_last_pos.x = xpos;
		app->cursor_last_pos.y = ypos;
		app->is_entering_free_flight = false;
		return;
	}
	app->camera.rotation.y -= (xpos - app->cursor_last_pos.x) / 5;
	app->camera.rotation.x -= (ypos - app->cursor_last_pos.y) / 5;

	if (app->camera.rotation.y < -360) {
		app->camera.rotation.y += 360;
	} else if (app->camera.rotation.y > 360) {
		app->camera.rotation.y -= 360;
	}
	if (app->camera.rotation.x < -89) {
		app->camera.rotation.x = -89;
	} else if (app->camera.rotation.x > 89) {
		app->camera.rotation.x = 89;
	}

	app->cursor_last_pos.x = xpos;
	app->cursor_last_pos.y = ypos;
}
