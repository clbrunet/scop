#ifndef SCOP_APP_H
#define SCOP_APP_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

typedef struct app_s
{
	GLFWwindow *window;
	GLuint program;
	GLuint color_uniform_location;
	GLuint x_displacement_uniform_location;
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
}	app_t;

#endif
