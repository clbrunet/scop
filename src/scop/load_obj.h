#ifndef SCOP_LOAD_OBJ_H
#define SCOP_LOAD_OBJ_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

typedef struct model_s model_t;

typedef enum face_format_e {
	ERROR,
	V,
	V_VT,
	V_VN,
	V_VT_VN,
}	face_format_t;

typedef struct polygon_s
{
	GLuint vertices_count;
	GLuint *vertices_index;
}	polygon_t;

int load_obj(model_t *model, const char *path);

#endif
