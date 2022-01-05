#ifndef SCOP_INITIALIZATION_H
#define SCOP_INITIALIZATION_H

#include <sys/types.h>

#include "scop/app.h"
#include "scop/vectors/vec3.h"

typedef GLuint triangle_t[3];

typedef struct vertex_s
{
	vec3_t position;
	vec3_t color;
	vec2_t texture_coordinates;
}	vertex_t;

typedef struct array_buffer_data_s
{
	GLuint vertices_count;
	vertex_t *vertices;
}	array_buffer_data_t;

typedef struct texture_s
{
	int width;
	int height;
	int channel_count;
	u_char *data;
}	texture_t;

typedef struct model_s
{
	GLuint vertices_count;
	vec3_t *vertices_position;
	GLuint triangles_count;
	triangle_t *triangles;
	bounding_box_t bounding_box;
}	model_t;

int initialization(app_t *app, const char *object_path, const char *texture_path);

#endif
