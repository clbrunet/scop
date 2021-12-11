#ifndef SCOP_INITIALIZATION_H
#define SCOP_INITIALIZATION_H

#include "scop/app.h"
#include "scop/vectors/vec3.h"

typedef GLuint triangle_t[3];

typedef struct vertex_s
{
	vec3_t position;
}	vertex_t;

typedef struct array_buffer_data_s
{
	GLuint vertex_count;
	vertex_t *vertices;
}	array_buffer_data_t;

typedef struct model_s
{
	GLuint vertex_count;
	vec3_t *vertices;
	GLuint triangle_count;
	triangle_t *triangles;
}	model_t;

int initialization(app_t *app, const char *object_path);

#endif
