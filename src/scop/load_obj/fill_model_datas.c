#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "scop/initialization.h"
#include "scop/load_obj.h"
#include "scop/load_obj/utils.h"
#include "scop/vectors/vec3.h"

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

int fill_model_datas(model_t *model, polygon_t *polygons, char **lines)
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
