#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>
#include <assert.h>

#include "scop/app.h"
#include "scop/initialization.h"
#include "scop/load_obj.h"
#include "scop/utils/file.h"
#include "scop/utils/string.h"
#include "scop/vectors/vec3.h"

static void count_model_datas_vertex(model_t *model, char *data)
{
	(void)data;
	model->vertices_count++;
}

static int count_model_datas_face(model_t *model, GLuint *polygons_count, char *data)
{
	GLuint count = 0;

	while (*data != '\0') {
		count++;
		while (*data != '\0' && !isspace(*data)) {
			data++;
		}
		while (isspace(*data)) {
			data++;
		}
	}
	if (count < 3) {
		return -1;
	}
	model->triangles_count += count - 2;
	*polygons_count += 1;
	return 0;
}

static int count_model_datas(model_t *model, GLuint *polygons_count, char **lines)
{
	model->vertices_count = 0;
	model->triangles_count = 0;
	*polygons_count = 0;

	while (*lines != NULL) {
		char keyword[3];
		int n;
		if (sscanf(*lines, "%2s %n", keyword, &n) == EOF) {
			lines++;
			continue;
		}

		if (strcmp(keyword, "v") == 0) {
			count_model_datas_vertex(model, *lines + n);
		} else if (strcmp(keyword, "f") == 0) {
			if (count_model_datas_face(model, polygons_count, *lines + n) == -1) {
				return -1;
			}
		}
		lines++;
	}
	if (model->triangles_count == 0) {
		return -1;
	}
	return 0;
}

static int fill_model_datas_vertex(vec3_t **vertices_it, char *data)
{
	int n;
	if (sscanf(data, "%f %f %f %n",
			&(*vertices_it)->x,
			&(*vertices_it)->y,
			&(*vertices_it)->z, &n) != 3 || data[n] != 0) {
		return -1;
	}
	*vertices_it += 1;
	return 0;
}

static face_format_t get_face_format(char *data)
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

static GLuint count_face_vertices(char *data, face_format_t face_format)
{
	assert(data != NULL);

	const char *format;
	switch (face_format) {
		case V:
			format = "%*u %n";
			break;
		case V_VT:
			format = "%*u/%*u %n";
			break;
		case V_VN:
			format = "%*u//%*u %n";
			break;
		case V_VT_VN:
			format = "%*u/%*u/%*u %n";
			break;
		default:
			assert(false);
	}

	int count = 0;
	int n = 0;
	sscanf(data, format, &n);
	while (n != 0) {
		count++;
		data += n;
		n = 0;
		sscanf(data, format, &n);
	}
	return count;
}

static int fill_face_vertices(polygon_t *polygon, char *data, face_format_t face_format,
		GLuint model_vertices_count)
{
	assert(polygon != NULL);
	assert(data != NULL);

	const char *format;
	switch (face_format) {
		case V:
			format = "%u %n";
			break;
		case V_VT:
			format = "%u/%*u %n";
			break;
		case V_VN:
			format = "%u//%*u %n";
			break;
		case V_VT_VN:
			format = "%u/%*u/%*u %n";
			break;
		default:
			assert(false);
	}

	GLuint *polygon_vertices_index_it = polygon->vertices_index;
	for (GLuint i = 0; i < polygon->vertices_count; i++) {
		int n = 0;
		sscanf(data, format, polygon_vertices_index_it, &n);
		if (*polygon_vertices_index_it > model_vertices_count) {
			return -1;
		}
		*polygon_vertices_index_it -= 1;
		data += n;
		polygon_vertices_index_it++;
	}
	return 0;
}

static int fill_model_datas_face(polygon_t **polygons_it, char *data, GLuint vertices_count)
{
	face_format_t face_format = get_face_format(data);
	if (face_format == ERROR) {
		return -1;
	}
	(*polygons_it)->vertices_count = count_face_vertices(data, face_format);
	(*polygons_it)->vertices_index = malloc((*polygons_it)->vertices_count * sizeof(GLuint));
	if ((*polygons_it)->vertices_index == NULL) {
		return -1;
	}
	if (fill_face_vertices(*polygons_it, data, face_format, vertices_count) == -1) {
		free((*polygons_it)->vertices_index);
		return -1;
	}
	*polygons_it += 1;
	return 0;
}

static int fill_model_datas(model_t *model, polygon_t *polygons, char **lines)
{
	vec3_t *vertices_it = model->vertices_position;
	polygon_t *polygons_it = polygons;

	while (*lines != NULL) {
		char keyword[3];
		int n;
		if (sscanf(*lines, "%2s %n", keyword, &n) == EOF) {
			lines++;
			continue;
		}
		if (strcmp(keyword, "v") == 0) {
			if (fill_model_datas_vertex(&vertices_it, *lines + n) == -1) {
				while (polygons < polygons_it) {
					free(polygons->vertices_index);
					polygons++;
				}
				return -1;
			}
		} else if (strcmp(keyword, "f") == 0) {
			if (fill_model_datas_face(&polygons_it, *lines + n, model->vertices_count) == -1) {
				while (polygons < polygons_it) {
					free(polygons->vertices_index);
					polygons++;
				}
				return -1;
			}
		}
		lines++;
	}
	return 0;
}

static void triangulate_polygon(polygon_t *polygon, triangle_t **triangles_it, const vec3_t *vertices_position)
{
	(**triangles_it)[0] = polygon->vertices_index[0];
	(**triangles_it)[1] = polygon->vertices_index[1];
	(**triangles_it)[2] = polygon->vertices_index[2];
	*triangles_it += 1;

	for (GLuint i = 3; i < polygon->vertices_count; i++) {
		(**triangles_it)[0] = (*triangles_it)[-1][0];
		(**triangles_it)[1] = (*triangles_it)[-1][2];
		(**triangles_it)[2] = polygon->vertices_index[i];
		*triangles_it += 1;
	}
}

static void triangulate_polygons(polygon_t *polygons_it, GLuint polygons_count, model_t *model)
{
	triangle_t *triangles_it = model->triangles;
	while (polygons_count > 0) {
		triangulate_polygon(polygons_it, &triangles_it, model->vertices_position);
		free(polygons_it->vertices_index);
		polygons_it++;
		polygons_count--;
	}
}

static void set_bounding_box_axes(model_t *model)
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
		if (model->bounding_box.x.max < vertices_it->y) {
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

static void center_bounding_box(model_t *model)
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

int load_obj(model_t *model, const char *path)
{
	const char *extension =  strrchr(path, '.');
	if (extension == NULL || strcmp(extension, ".obj") != 0) {
		fprintf(stderr, "Object '%s' must have '.obj' extension.\n", path);
		return -1;
	}
	char *file_content = read_file(path, NULL);
	if (file_content == NULL) {
		return -1;
	}
	char **lines = split(file_content, '\n');
	free(file_content);
	if (lines == NULL) {
		return -1;
	}

	GLuint polygons_count;
	polygon_t *polygons;
	if (count_model_datas(model, &polygons_count, lines) == -1) {
		fprintf(stderr, "Object '%s' has invalid lines.\n", path);
		free_strs(lines);
		return -1;
	}
	model->vertices_position = malloc(model->vertices_count * sizeof(vec3_t));
	model->triangles = malloc(model->triangles_count * sizeof(triangle_t));
	polygons = malloc(polygons_count * sizeof(polygon_t));
	if (model->vertices_position == NULL || model->triangles == NULL) {
		err(1, "malloc");
		free(model->vertices_position);
		free(model->triangles);
		free(polygons);
		free_strs(lines);
		return -1;
	}
	if (fill_model_datas(model, polygons, lines) == -1) {
		fprintf(stderr, "Object '%s' has invalid lines.\n", path);
		free(model->vertices_position);
		free(model->triangles);
		free(polygons);
		free_strs(lines);
		return -1;
	}
	free_strs(lines);

	triangulate_polygons(polygons, polygons_count, model);
	free(polygons);
	center_bounding_box(model);
	return 0;
}
