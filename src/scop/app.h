#ifndef SCOP_APP_H
#define SCOP_APP_H

#include <stdbool.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/vectors/vec2.h"
#include "scop/vectors/vec3.h"
#include "scop/vectors/vec2_double.h"

typedef struct camera_s
{
	vec3_t position;
	vec2_t rotation;
}	camera_t;

typedef struct update_time_s
{
	GLdouble current;
	GLdouble delta;
}	update_time_t;

typedef struct uniforms_s
{
	GLint projection_view_model;
	GLint texture_portion;
}	uniforms_t;

typedef struct range_s
{
	GLfloat min;
	GLfloat max;
}	range_t;

typedef struct bounding_box_s
{
	range_t x;
	range_t y;
	range_t z;
	GLfloat max_distance;
}	bounding_box_t;

typedef enum texture_animation_phase_e
{
	TO_COLOR,
	TO_TEXTURE,
}	texture_animation_phase_t;

typedef struct window_s
{
	GLFWwindow *ptr;
	GLsizei width;
	GLsizei height;
}	window_t;

typedef struct opengl_s
{
	GLuint program;
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint texture_map;
	uniforms_t uniforms;
}	opengl_t;

typedef struct app_s
{
	window_t window;
	opengl_t opengl;
	update_time_t time;
	GLfloat fov;
	bool should_use_orthographic;
	camera_t camera;
	bool is_entering_free_flight;
	vec2_double_t cursor_last_pos;
	GLsizei model_triangle_count;
	bounding_box_t model_bounding_box;
	GLfloat model_y_rotation;
	bool should_model_rotate;
	texture_animation_phase_t texture_animation_phase;
	GLfloat texture_portion;
}	app_t;

#endif
