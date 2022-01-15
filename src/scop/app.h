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
	GLfloat speed;
}	camera_t;

typedef struct update_time_s
{
	GLdouble current;
	GLdouble delta;
}	update_time_t;

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

typedef struct normals_program_s
{
	GLuint id;
	GLint view_model;
	GLint projection;
}	normals_program_t;

typedef struct model_program_s
{
	GLuint id;
	GLint projection_view_model;
	GLint model_color;
	GLint texture_portion;
}	model_program_t;

typedef struct model_lighting_program_s
{
	GLuint id;
	GLint material_diffuse;
	GLint view_model;
	GLint projection_view_model;
	GLint texture_portion;
	GLint light_position;
}	model_lighting_program_t;

typedef struct opengl_s
{
	normals_program_t normals_program;
	model_program_t model_program;
	model_lighting_program_t model_lighting_program;
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint texture_map;
}	opengl_t;

typedef struct model_info_s
{
	GLsizei triangles_count;
	bounding_box_t bounding_box;
	GLfloat yaw;
	bool should_rotate;
	vec3_t position;
	vec3_t color;
}	model_info_t;

typedef enum axis_e
{
	X,
	Y,
	Z,
}	axis_t;

typedef struct light_s
{
	vec3_t position;
	vec3_t ambient;
	vec3_t diffuse;
	vec3_t specular;
}	light_t;

typedef struct app_s
{
	window_t window;
	opengl_t opengl;
	update_time_t time;
	GLfloat fov;
	GLenum polygon_mode;
	bool should_use_orthographic;
	bool should_display_normals;
	bool should_use_lighting;
	camera_t camera;
	light_t light;
	bool is_entering_free_flight;
	vec2_double_t cursor_last_pos;
	model_info_t model_info;
	texture_animation_phase_t texture_animation_phase;
	GLfloat texture_portion;
	axis_t selected_axis;
}	app_t;

#endif
