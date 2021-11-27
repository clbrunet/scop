#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scop/initialization.h"
#include "scop/vectors/vec3.h"

// @todo .obj checker

static int count_triangle(char *face_datas)
{
	int count = 0;

	while (isspace(*face_datas)) {
		face_datas++;
	}
	while (*face_datas != '\0') {
		count++;
		while (*face_datas != '\0' && !isspace(*face_datas)) {
			face_datas++;
		}
		while (isspace(*face_datas)) {
			face_datas++;
		}
	}
	return count - 2;
}

static void count_model_datas(model_t *model, FILE *file)
{
	model->vertex_count = 0;
	model->triangle_count = 0;
	char *line;
	fscanf(file, " %m[^\n]", &line);
	while (line != NULL) {
		if (strncmp(line, "v ", 2) == 0) {
			model->vertex_count++;
		} else if (strncmp(line, "f ", 2) == 0) {
			model->triangle_count += count_triangle(line + 2);
		}
		free(line);
		fscanf(file, " %m[^\n]", &line);
	}
}

static int fill_model_datas_face(char *face_datas, triangle_t *triangles, int *triangle_i)
{
	int n;

	while (isspace(*face_datas)) {
		face_datas++;
	}
	for (size_t i = 0; i < 3; i++) {
		if (sscanf(face_datas, "%u%n", &triangles[*triangle_i][i], &n) != 1) {
			return -1;
		}
		triangles[*triangle_i][i]--;
		face_datas += n;
		while (*face_datas != '\0' && !isspace(*face_datas)) {
			face_datas++;
		}
		while (isspace(*face_datas)) {
			face_datas++;
		}
	}
	*triangle_i += 1;
	while (*face_datas != '\0') {
		triangles[*triangle_i][0] = triangles[*triangle_i - 1][0];
		triangles[*triangle_i][1] = triangles[*triangle_i - 1][2];
		if (sscanf(face_datas, "%u%n", &triangles[*triangle_i][2], &n) != 1) {
			return -1;
		}
		triangles[*triangle_i][2]--;
		face_datas += n;
		while (*face_datas != '\0' && !isspace(*face_datas)) {
			face_datas++;
		}
		while (isspace(*face_datas)) {
			face_datas++;
		}
		*triangle_i += 1;
	}
	return 0;
}

static int fill_model_datas(model_t *model, FILE *file)
{
	int vertex_i = 0;
	int triangle_i = 0;
	char *line;
	fscanf(file, " %m[^\n]", &line);
	while (line != NULL) {
		if (strncmp(line, "v ", 2) == 0) {
			if (sscanf(line + 2, "%f %f %f",
					&model->vertices[vertex_i].x,
					&model->vertices[vertex_i].y,
					&model->vertices[vertex_i].z) != 3) {
				free(line);
				return -1;
			}
			vertex_i++;
		} else if (strncmp(line, "f ", 2) == 0) {
			fill_model_datas_face(line + 2, model->triangles, &triangle_i);
		}
		free(line);
		fscanf(file, " %m[^\n]", &line);
	}
	return 0;
}

int load_model(model_t *model, const char *object_path)
{
	FILE *file = fopen(object_path, "rb");
	if (file == NULL) {
		return -1;
	}
	// @todo .obj checker
	count_model_datas(model, file);
	model->vertices = malloc(model->vertex_count * sizeof(vec3_t));
	model->triangles = malloc(model->triangle_count * sizeof(triangle_t));
	if (model->vertices == NULL || model->triangles == NULL) {
		free(model->vertices);
		free(model->triangles);
		fclose(file);
		return -1;
	}
	fseek(file, 0, SEEK_SET);
	fill_model_datas(model, file);
	fclose(file);
	return 0;
}
