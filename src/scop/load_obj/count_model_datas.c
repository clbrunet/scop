#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "scop/initialization.h"

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

int count_model_datas(model_t *model, GLuint *polygons_count, char **lines)
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
