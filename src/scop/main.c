#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"
#include "scop/initialization.h"
#include "scop/update.h"
#include "scop/destruction.h"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	app_t app = {0};

	if (initialization(&app) == -1) {
		return 1;
	}

	double last_frame_time = glfwGetTime();;
	while (glfwWindowShouldClose(app.window) == GLFW_FALSE) {
		app.current_time = glfwGetTime();
		app.delta_time = app.current_time - last_frame_time;

		update(&app);

		glfwSwapBuffers(app.window);
		glfwPollEvents();
		last_frame_time = app.current_time;
	}

	destruction(&app);
	return 0;
}
