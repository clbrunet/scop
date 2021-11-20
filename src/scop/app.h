#ifndef SCOP_APP_H
#define SCOP_APP_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

typedef struct app_s
{
	GLFWwindow *window;
	GLsizei window_width;
	GLsizei window_height;
	GLuint program;
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
	GLsizei triangle_count;
}	app_t;

#endif
