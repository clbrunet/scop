#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include <unistd.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/load_tga.h"
#include "scop/initialization.h"
#include "scop/update.h"
#include "scop/destruction.h"
#include "scop/style.h"

static int update_fps(const app_t *app, int *frame_count, int *last_fps_print)
{
	int length = snprintf(NULL, 0, "SCOP %f", (float)*frame_count / (app->time.current - *last_fps_print));
	char *title = malloc((length + 1) * sizeof(char));
	if (title == NULL) {
		err(1, "malloc");
		return -1;
	}
	sprintf(title, "SCOP %.1f", (float)*frame_count / (app->time.current - *last_fps_print));
	glfwSetWindowTitle(app->window, title);
	free(title);
	*frame_count = 0;
	*last_fps_print = app->time.current;
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s " UNDERLINED "OBJECT.obj" RESET_UNDERLINED " "
				UNDERLINED "TEXTURE.tga" RESET_UNDERLINED "\n", argv[0]);
		return 0;
	}

	app_t app = {};
	if (initialization(&app, argv[1], argv[2]) == -1) {
		return 1;
	}

	GLdouble last_frame_time = glfwGetTime();
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	GLdouble last_swap_time = glfwGetTime();
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	int frame_count = 0;
	int last_fps_print = glfwGetTime();
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	while (glfwWindowShouldClose(app.window) == GLFW_FALSE) {
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		app.time.current = glfwGetTime();
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		app.time.delta = app.time.current - last_frame_time;
		if (app.time.current - last_fps_print > 1) {
			if (update_fps(&app, &frame_count, &last_fps_print) == -1) {
				break;
			}
		}

		update(&app);

		while (glfwGetTime() - last_swap_time < (GLdouble)1 / 60) {
			usleep(100);
		}
		glfwSwapBuffers(app.window);
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		last_swap_time = glfwGetTime();
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		glfwPollEvents();
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		last_frame_time = app.time.current;
		frame_count++;
	}

	destruction(&app);
	return 0;
}
