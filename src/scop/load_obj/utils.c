#include <stdio.h>

#include "scop/load_obj.h"
#include "scop/initialization.h"

face_format_t get_face_format(char *data)
{
	int n = 0;

	sscanf(data, "%*u %*u %*u %n", &n);
	if (n != 0) {
		return V;
	}
	sscanf(data, "%*u/%*u %*u/%*u %*u/%*u %n", &n);
	if (n != 0) {
		return V_VT;
	}
	sscanf(data, "%*u//%*u %*u//%*u %*u//%*u %n", &n);
	if (n != 0) {
		return V_VN;
	}
	sscanf(data, "%*u/%*u/%*u %*u/%*u/%*u %*u/%*u/%*u %n", &n);
	if (n != 0) {
		return V_VT_VN;
	}
	return ERROR;
}

void set_bounding_box_axes(model_t *model)
{
	model->bounding_box = (bounding_box_t){
		.x.min = model->vertices_position->x, .x.max = model->vertices_position->x,
			.y.min = model->vertices_position->y, .y.max = model->vertices_position->y,
			.z.min = model->vertices_position->z, .z.max = model->vertices_position->z,
	};

	vec3_t *vertices_it = model->vertices_position + 1;
	for (GLuint i = 1; i < model->vertices_count; i++) {
		if (vertices_it->x < model->bounding_box.x.min) {
			model->bounding_box.x.min = vertices_it->x;
		}
		if (model->bounding_box.x.max < vertices_it->x) {
			model->bounding_box.x.max = vertices_it->x;
		}
		if (vertices_it->y < model->bounding_box.y.min) {
			model->bounding_box.y.min = vertices_it->y;
		}
		if (model->bounding_box.y.max < vertices_it->y) {
			model->bounding_box.y.max = vertices_it->y;
		}
		if (vertices_it->z < model->bounding_box.z.min) {
			model->bounding_box.z.min = vertices_it->z;
		}
		if (model->bounding_box.z.max < vertices_it->z) {
			model->bounding_box.z.max = vertices_it->z;
		}
		vertices_it++;
	}
}

void center_bounding_box(model_t *model)
{
	set_bounding_box_axes(model);

	vec3_t center = {
		.x = (model->bounding_box.x.min + model->bounding_box.x.max) / 2,
		.y = (model->bounding_box.y.min + model->bounding_box.y.max) / 2,
		.z = (model->bounding_box.z.min + model->bounding_box.z.max) / 2,
	};

	vec3_t *vertices_it = model->vertices_position;
	for (GLuint i = 0; i < model->vertices_count; i++) {
		vertices_it->x -= center.x;
		vertices_it->y -= center.y;
		vertices_it->z -= center.z;
		vertices_it++;
	}

	model->bounding_box.x.min -= center.x;
	model->bounding_box.x.max -= center.x;
	model->bounding_box.y.min -= center.y;
	model->bounding_box.y.max -= center.y;
	model->bounding_box.z.min -= center.z;
	model->bounding_box.z.max -= center.z;

	if (model->bounding_box.x.max > model->bounding_box.y.max) {
		if (model->bounding_box.x.max > model->bounding_box.z.max) {
			model->bounding_box.max_distance = model->bounding_box.x.max;
		} else {
			model->bounding_box.max_distance = model->bounding_box.z.max;
		}
	} else {
		if (model->bounding_box.y.max > model->bounding_box.z.max) {
			model->bounding_box.max_distance = model->bounding_box.y.max;
		} else {
			model->bounding_box.max_distance = model->bounding_box.z.max;
		}
	}
}
