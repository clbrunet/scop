#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

#include "scop/initialization.h"
#include "scop/utils/file.h"
#include "scop/utils/string.h"
#include "scop/vectors/vec3.h"

// @todo .obj checker

static void count_model_datas_vertex(model_t *model)
{
	model->vertex_count++;
}

static void count_model_datas_face(model_t *model, char *data)
{
	int count = 0;

	while (isspace(*data)) {
		data++;
	}
	while (*data != '\0') {
		count++;
		while (*data != '\0' && !isspace(*data)) {
			data++;
		}
		while (isspace(*data)) {
			data++;
		}
	}
	model->triangle_count += count - 2;
}

static void count_model_datas(model_t *model, char **lines)
{
	model->vertex_count = 0;
	model->triangle_count = 0;

	while (*lines != NULL) {
		char keyword[3];
		int n;
		if (sscanf(*lines, "%2s%n", keyword, &n) == EOF) {
			lines++;
			continue;
		}
		if (strcmp(keyword, "v") == 0) {
			count_model_datas_vertex(model);
		} else if (strcmp(keyword, "f") == 0) {
			count_model_datas_face(model, *lines + n + 1);
		}
		lines++;
	}
}

static void fill_model_datas_vertex(vec3_t **vertices_it, char *data)
{
	sscanf(data, "%f %f %f",
			&(*vertices_it)->x,
			&(*vertices_it)->y,
			&(*vertices_it)->z);
	*vertices_it += 1;
}

static void fill_model_datas_face(triangle_t **triangles_it, char *data)
{
	int n;

	for (size_t i = 0; i < 3; i++) {
		sscanf(data, "%u%n", &(**triangles_it)[i], &n);
		(**triangles_it)[i]--;
		data += n;
		while (*data != '\0' && !isspace(*data)) {
			data++;
		}
		while (isspace(*data)) {
			data++;
		}
	}
	*triangles_it += 1;
	while (*data != '\0') {
		(**triangles_it)[0] = ((*triangles_it)[-1])[0];
		(**triangles_it)[1] = ((*triangles_it)[-1])[2];
		sscanf(data, "%u%n", &(**triangles_it)[2], &n);
		(**triangles_it)[2]--;
		data += n;
		while (*data != '\0' && !isspace(*data)) {
			data++;
		}
		while (isspace(*data)) {
			data++;
		}
		*triangles_it += 1;
	}
}

static void fill_model_datas(model_t *model, char **lines)
{
	vec3_t *vertices_it = model->vertices;
	triangle_t *triangles_it = model->triangles;

	while (*lines != NULL) {
		char keyword[3];
		int n;
		if (sscanf(*lines, "%2s%n", keyword, &n) == EOF) {
			lines++;
			continue;
		}
		if (strcmp(keyword, "v") == 0) {
			fill_model_datas_vertex(&vertices_it, *lines + n + 1);
		} else if (strcmp(keyword, "f") == 0) {
			fill_model_datas_face(&triangles_it, *lines + n + 1);
		}
		lines++;
	}
}

static void  center_model(model_t *model)
{
	vec3_t mean = {};
	vec3_t *vertices_it;

	vertices_it = model->vertices;
	for (GLsizei i = 0; i < model->vertex_count; i++) {
		mean.x += vertices_it->x;
		mean.y += vertices_it->y;
		mean.z += vertices_it->z;
		vertices_it++;
	}
	mean.x /= model->vertex_count;
	mean.y /= model->vertex_count;
	mean.z /= model->vertex_count;
	vertices_it = model->vertices;
	for (GLsizei i = 0; i < model->vertex_count; i++) {
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

	// @todo .obj checker

	count_model_datas(model, lines);
	model->vertices = malloc(model->vertex_count * sizeof(vec3_t));
	model->triangles = malloc(model->triangle_count * sizeof(triangle_t));
	if (model->vertices == NULL || model->triangles == NULL) {
		err(1, "malloc");
		free(model->vertices);
		free(model->triangles);
		free_strs(lines);
		return -1;
	}
	fill_model_datas(model, lines);
	free_strs(lines);

	center_model(model);
	return 0;
}
