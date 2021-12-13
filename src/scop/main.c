#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/load_tga.h"
#include "scop/initialization.h"
#include "scop/update.h"
#include "scop/destruction.h"
#include "scop/style.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s " UNDERLINED "OBJECT" RESET_UNDERLINED "\n", argv[0]);
		return 0;
	}
	app_t app = {};

	if (initialization(&app, argv[1]) == -1) {
		return 1;
	}

	GLdouble last_frame_time = glfwGetTime();
	assert(glfwGetError(NULL) == GLFW_NO_ERROR);
	int frame_count = 0;
	int last_fps_print = last_frame_time;
	while (glfwWindowShouldClose(app.window) == GLFW_FALSE) {
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		app.time.current = glfwGetTime();
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		app.time.delta = app.time.current - last_frame_time;
		if (app.time.current - last_fps_print > 1) {
			int length = snprintf(NULL, 0, "SCOP %f", (float)frame_count / (app.time.current - last_fps_print));
			char *title = malloc((length + 1) * sizeof(char));
			if (title == NULL) {
				err(1, "malloc");
				break;
			}
			sprintf(title, "SCOP %.1f", (float)frame_count / (app.time.current - last_fps_print));
			glfwSetWindowTitle(app.window, title);
			free(title);
			frame_count = 0;
			last_fps_print = app.time.current;
		}

		update(&app);

		glfwSwapBuffers(app.window);
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		glfwPollEvents();
		assert(glfwGetError(NULL) == GLFW_NO_ERROR);
		last_frame_time = app.time.current;
		frame_count++;
	}

	destruction(&app);
	return 0;
}
