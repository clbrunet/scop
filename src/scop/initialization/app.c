#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/initialization.h"
#include "scop/initialization/utils.h"
#include "scop/app.h"
#include "scop/load_obj.h"
#include "scop/load_tga.h"
#include "scop/destruction.h"
#include "scop/vectors/vec3.h"
#include "scop/utils/math.h"

static void initialize_variables(app_t *app)
{
	app->window.width = 1280;
	app->window.height = 720;

	app->fov = 90;
	app->polygon_mode = GL_FILL;
	app->should_use_orthographic = false;
	app->should_display_normals = false;
	app->should_use_lighting = false;

	app->camera.speed = 8;

	app->light.ambient = (vec3_t){ .r = 0.1, .g = 0.1, .b = 0.1, };
	app->light.diffuse = (vec3_t){ .r = 0.8, .g = 0.8, .b = 0.8, };
	app->light.specular = (vec3_t){ .r = 0.2, .g = 0.2, .b = 0.2, };

	app->is_entering_free_flight = false;

	app->model_info.should_rotate = true;
	app->model_info.position = (vec3_t){ .x = 0, .y = 0, .z = 0 };
	app->model_info.color = (vec3_t){ .x = 1, .y = 1, .z = 1 };

	app->texture_animation_phase = TO_COLOR;
	app->texture_portion = 0;

	app->selected_axis = X;
}

// horizontal_fov in radians
static void set_camera_position(vec3_t *camera_position,
		const bounding_box_t *model_bounding_box, GLfloat horizontal_fov, GLfloat aspect_ratio)
{
	GLfloat projection_plane_distance = aspect_ratio / tan(horizontal_fov / 2);

	GLfloat rightmost = sqrtf(powf(model_bounding_box->x.max, 2)
			+ powf(model_bounding_box->z.max, 2));
	GLfloat upmost = sqrtf(powf(rightmost, 2) + powf(model_bounding_box->y.max, 2));

	GLfloat up = upmost * projection_plane_distance;
	GLfloat right = rightmost * projection_plane_distance / aspect_ratio;

	camera_position->z = fmaxf(up, right);
	camera_position->z += rightmost;
}

int initialization(app_t *app, const char *object_path, const char *texture_path)
{
	initialize_variables(app);
	if (initialize_glfw(app) == -1) {
		return -1;
	}
	if (initialize_gl(app) == -1) {
		glfwTerminate();
		return -1;
	}

	model_t model;
	if (load_obj(&model, object_path) == -1) {
		destruction(app);
		return -1;
	}
	app->model_info.triangles_count = model.triangles_count;
	app->model_info.bounding_box = model.bounding_box;

	texture_t texture;
	texture.data = load_tga(texture_path, &texture.width, &texture.height,
			&texture.channel_count);
	if (texture.data == NULL) {
		free(model.vertices_position);
		free(model.triangles);
		destruction(app);
		return -1;
	}

	if (initialize_model_vao(app, &model, &texture) == -1) {
		free(model.vertices_position);
		free(model.triangles);
		free(texture.data);
		destruction(app);
		return -1;
	}
	free(model.vertices_position);
	free(model.triangles);
	free(texture.data);

	set_camera_position(&app->camera.position, &app->model_info.bounding_box,
			radians(app->fov), (GLfloat)app->window.width / (GLfloat)app->window.height);
	app->light.position = (vec3_t){
		.x = app->model_info.bounding_box.max_distance,
		.y = app->model_info.bounding_box.max_distance,
		.z = app->camera.position.z,
	};
	if (initialize_light_vao(app) == -1) {
		destruction(app);
		return -1;
	}

	return 0;
}
