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
#include "scop/load_obj/utils.h"

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
