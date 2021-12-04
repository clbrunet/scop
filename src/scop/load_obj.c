#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

#include "scop/initialization.h"
#include "scop/load_obj.h"
#include "scop/utils/file.h"
#include "scop/utils/string.h"
#include "scop/vectors/vec3.h"

static void count_model_datas_vertex(model_t *model, char *data)
{
	(void)data;
	model->vertex_count++;
}

static int count_model_datas_face(model_t *model, char *data)
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
	model->triangle_count += count - 2;
	return 0;
}

static int count_model_datas(model_t *model, char **lines)
{
	model->vertex_count = 0;
	model->triangle_count = 0;

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
			if (count_model_datas_face(model, *lines + n) == -1) {
				return -1;
			}
		}
		lines++;
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

static int fill_model_datas_face(triangle_t **triangles_it, char *data, GLuint vertex_count)
{
	face_format_t face_format;
	int n;
	GLuint scanf_uint;

	if (sscanf(data, "%u %u %u %n", &(**triangles_it)[0], &(**triangles_it)[1],
			&(**triangles_it)[2], &n) == 3) {
		face_format = V;
	} else if (sscanf(data, "%u/%u %u/%u %u/%u %n", &(**triangles_it)[0], &scanf_uint,
			&(**triangles_it)[1], &scanf_uint, &(**triangles_it)[2], &scanf_uint, &n) == 6) {
		face_format = V_VT;
	} else if (sscanf(data, "%u//%u %u//%u %u//%u %n", &(**triangles_it)[0], &scanf_uint,
			&(**triangles_it)[1], &scanf_uint, &(**triangles_it)[2], &scanf_uint, &n) == 6) {
		face_format = V_VN;
	} else if (sscanf(data, "%u/%u/%u %u/%u/%u %u/%u/%u %n", &(**triangles_it)[0], &scanf_uint,
			&scanf_uint, &(**triangles_it)[1], &scanf_uint, &scanf_uint, &(**triangles_it)[2],
			&scanf_uint, &scanf_uint, &n) == 9) {
		face_format = V_VT_VN;
	} else {
		return -1;
	}
	if ((**triangles_it)[0] > vertex_count || (**triangles_it)[1] > vertex_count
		|| (**triangles_it)[2] > vertex_count) {
		return -1;
	}
	(**triangles_it)[0]--;
	(**triangles_it)[1]--;
	(**triangles_it)[2]--;
	data += n;
	*triangles_it += 1;

	while (*data != '\0') {
		(**triangles_it)[0] = ((*triangles_it)[-1])[0];
		(**triangles_it)[1] = ((*triangles_it)[-1])[2];
		if (face_format == V) {
			if (sscanf(data, "%u %n", &(**triangles_it)[2], &n) != 1) {
				return -1;
			}
		} else if (true) {}
		switch (face_format) {
			case V:
				if (sscanf(data, "%u %n", &(**triangles_it)[2], &n) != 1) {
					return -1;
				}
				break;
			case V_VT:
				if (sscanf(data, "%u/%u %n", &(**triangles_it)[2], &scanf_uint, &n) != 2) {
					return -1;
				}
				break;
			case V_VN:
				if (sscanf(data, "%u//%u %n", &(**triangles_it)[2], &scanf_uint, &n) != 2) {
					return -1;
				}
				break;
			case V_VT_VN:
				if (sscanf(data, "%u/%u/%u %n", &(**triangles_it)[2], &scanf_uint, &scanf_uint, &n) != 3) {
					return -1;
				}
				break;
		}
		if ((**triangles_it)[2] > vertex_count) {
			return -1;
		}
		(**triangles_it)[2]--;
		data += n;
		*triangles_it += 1;
	}
	return 0;
}

static int fill_model_datas(model_t *model, char **lines)
{
	vec3_t *vertices_it = model->vertices;
	triangle_t *triangles_it = model->triangles;

	while (*lines != NULL) {
		char keyword[3];
		int n;
		if (sscanf(*lines, "%2s %n", keyword, &n) == EOF) {
			lines++;
			continue;
		}
		if (strcmp(keyword, "v") == 0) {
			if (fill_model_datas_vertex(&vertices_it, *lines + n) == -1) {
				return -1;
			}
		} else if (strcmp(keyword, "f") == 0) {
			if (fill_model_datas_face(&triangles_it, *lines + n, model->vertex_count) == -1) {
				return -1;
			}
		}
		lines++;
	}
	return 0;
}

static void  center_model(model_t *model)
{
	vec3_t mean = {};
	vec3_t *vertices_it;

	vertices_it = model->vertices;
	for (GLuint i = 0; i < model->vertex_count; i++) {
		mean.x += vertices_it->x;
		mean.y += vertices_it->y;
		mean.z += vertices_it->z;
		vertices_it++;
	}
	mean.x /= model->vertex_count;
	mean.y /= model->vertex_count;
	mean.z /= model->vertex_count;
	vertices_it = model->vertices;
	for (GLuint i = 0; i < model->vertex_count; i++) {
		vertices_it->x -= mean.x;
		vertices_it->y -= mean.y;
		vertices_it->z -= mean.z;
		vertices_it++;
	}
}

int load_obj(model_t *model, const char *path)
{
	char *file_content = read_file(path, NULL);
	if (file_content == NULL) {
		return -1;
	}
	char **lines = split(file_content, '\n');
	free(file_content);
	if (lines == NULL) {
		return -1;
	}

	if (count_model_datas(model, lines) == -1) {
		fprintf(stderr, "Object '%s' has invalid lines.\n", path);
		free_strs(lines);
		return -1;
	}
	model->vertices = malloc(model->vertex_count * sizeof(vec3_t));
	model->triangles = malloc(model->triangle_count * sizeof(triangle_t));
	if (model->vertices == NULL || model->triangles == NULL) {
		err(1, "malloc");
		free(model->vertices);
		free(model->triangles);
		free_strs(lines);
		return -1;
	}
	if (fill_model_datas(model, lines) == -1) {
		fprintf(stderr, "Object '%s' has invalid lines.\n", path);
		free(model->vertices);
		free(model->triangles);
		free_strs(lines);
		return -1;
	}
	free_strs(lines);

	center_model(model);
	return 0;
}
