#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <err.h>

#include "scop/initialization.h"
#include "scop/app.h"
#include "scop/utils/math.h"

static vec2_t get_texture_coordinates(const vec3_t *position, const vec3_t *normal,
		const bounding_box_t *model_bounding_box, GLfloat texture_aspect_ratio)
{
	vec2_t texture_coordinates = {};

	vec3_t abs_normal = {
		.x = fabsf(normal->x),
		.y = fabsf(normal->y),
		.z = fabsf(normal->z),
	};
	if (abs_normal.x > abs_normal.y && abs_normal.x > abs_normal.z) {
		texture_coordinates.u = (-position->z - model_bounding_box->z.min) / texture_aspect_ratio;
		texture_coordinates.v = position->y - model_bounding_box->y.min;
	} else if (abs_normal.y > abs_normal.x && abs_normal.y > abs_normal.z) {
		texture_coordinates.u = (position->x - model_bounding_box->x.min) / texture_aspect_ratio;
		texture_coordinates.v = -position->z - model_bounding_box->z.min;
	} else {
		texture_coordinates.u = (position->x - model_bounding_box->x.min) / texture_aspect_ratio;
		texture_coordinates.v = position->y - model_bounding_box->y.min;
	}
	return texture_coordinates;
}

static int initialize_array_buffer_data(array_buffer_data_t *array_buffer_data,
		const model_t *model, GLfloat texture_aspect_ratio)
{
	array_buffer_data->vertices_count = model->triangles_count * 3;
	array_buffer_data->vertices = malloc(array_buffer_data->vertices_count * sizeof(vertex_t));
	if (array_buffer_data->vertices == NULL) {
		err(1, "malloc");
		return -1;
	}

	GLfloat whiteness = 1;
	GLfloat whiteness_shift = -0.04;
	vertex_t *vertex_it = array_buffer_data->vertices;
	triangle_t *triangle_it = model->triangles;
	for (GLuint i = 0; i < model->triangles_count; i++) {
		vec3_t color = vec3(whiteness, whiteness, whiteness);
		vec3_t normal = get_triangle_normal(&model->vertices_position[(*triangle_it)[0]],
				&model->vertices_position[(*triangle_it)[1]],
				&model->vertices_position[(*triangle_it)[2]]);
		vertex_it[0].position = model->vertices_position[(*triangle_it)[0]];
		vertex_it[0].color = color;
		vertex_it[0].texture_coordinates = get_texture_coordinates(
				&vertex_it[0].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[0].normal = normal;
		vertex_it[1].position = model->vertices_position[(*triangle_it)[1]];
		vertex_it[1].color = color;
		vertex_it[1].texture_coordinates = get_texture_coordinates(
				&vertex_it[1].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[1].normal = normal;
		vertex_it[2].position = model->vertices_position[(*triangle_it)[2]];
		vertex_it[2].color = color;
		vertex_it[2].texture_coordinates = get_texture_coordinates(
				&vertex_it[2].position, &normal, &model->bounding_box, texture_aspect_ratio);
		vertex_it[2].normal = normal;

		vertex_it += 3;
		triangle_it++;
		whiteness += whiteness_shift;
		if (whiteness < 0.3 || 1 < whiteness) {
			whiteness_shift = -whiteness_shift;
			whiteness += whiteness_shift;
		}
	}
	return 0;
}

static int initialize_array_buffer(const app_t *app, const array_buffer_data_t *array_buffer_data)
{
	glBindBuffer(GL_ARRAY_BUFFER, app->opengl.model_vao.vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, array_buffer_data->vertices_count * sizeof(vertex_t),
			array_buffer_data->vertices, GL_STATIC_DRAW);
	GLenum error = glGetError();
	if (error == GL_OUT_OF_MEMORY) {
		fprintf(stderr, "glBufferData out of memory error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);
	return 0;
}

static void initialize_vertex_array()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, position));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(0);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, color));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, texture_coordinates));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(2);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
			(const GLvoid *)offsetof(vertex_t, normal));
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(3);
	assert(glGetError() == GL_NO_ERROR);
}

static int initialize_texture_map(const app_t *app, const texture_t *texture)
{
	glActiveTexture(GL_TEXTURE0);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, app->opengl.model_vao.texture_map);
	assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	assert(glGetError() == GL_NO_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	assert(glGetError() == GL_NO_ERROR);
	if (texture->channel_count == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_BGR,
				GL_UNSIGNED_BYTE, texture->data);
	} else if (texture->channel_count == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_BGRA,
				GL_UNSIGNED_BYTE, texture->data);
	} else {
		fprintf(stderr, "Support only 3 or 4 texture channel count.\n");
		return -1;
	}
	GLenum error = glGetError();
	if (error == GL_INVALID_VALUE) {
		if ((u_int)texture->width > GL_MAX_TEXTURE_SIZE
			|| (u_int)texture->height > GL_MAX_TEXTURE_SIZE) {
			fprintf(stderr, "Texture too large.\n");
			return -1;
		}
	}
	assert(error == GL_NO_ERROR);
	glGenerateMipmap(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);

	GLint sampler = glGetUniformLocation(app->opengl.model_program.id, "sampler");
	assert(sampler != -1);
	glUseProgram(app->opengl.model_program.id);
	error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);
	glUniform1i(sampler, 0);
	assert(glGetError() == GL_NO_ERROR);

	GLint material_diffuse_maps = glGetUniformLocation(
			app->opengl.model_lighting_program.id, "material.diffuse_maps");
	assert(material_diffuse_maps != -1);
	glUseProgram(app->opengl.model_lighting_program.id);
	error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);
	glUniform1i(material_diffuse_maps, 0);
	assert(glGetError() == GL_NO_ERROR);

	return 0;
}

int initialize_model_vao(app_t *app, model_t *model, texture_t *texture)
{
	array_buffer_data_t array_buffer_data;
	if (initialize_array_buffer_data(&array_buffer_data, model,
			(GLfloat)texture->width / (GLfloat)texture->height) == -1) {
		return -1;
	}

	glBindVertexArray(app->opengl.model_vao.id);
	assert(glGetError() == GL_NO_ERROR);

	if (initialize_array_buffer(app, &array_buffer_data) == -1) {
		return -1;
	}
	free(array_buffer_data.vertices);
	initialize_vertex_array();
	if (initialize_texture_map(app, texture) == -1) {
		return -1;
	}
	return 0;
}

int initialize_light_vao(app_t *app)
{
	glBindVertexArray(app->opengl.light_vao.id);
	assert(glGetError() == GL_NO_ERROR);

	glBindBuffer(GL_ARRAY_BUFFER, app->opengl.light_vao.vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3_t), &app->light.position, GL_STATIC_DRAW);
	GLenum error = glGetError();
	if (error == GL_OUT_OF_MEMORY) {
		fprintf(stderr, "glBufferData out of memory error\n");
		return -1;
	}
	assert(error == GL_NO_ERROR);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3_t), 0);
	assert(glGetError() == GL_NO_ERROR);
	glEnableVertexAttribArray(0);
	assert(glGetError() == GL_NO_ERROR);

	return 0;
}
