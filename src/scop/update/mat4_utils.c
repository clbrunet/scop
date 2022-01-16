#include "scop/update.h"
#include "scop/app.h"
#include "scop/matrices/mat4.h"
#include "scop/utils/math.h"

void set_model_mat4(app_t *app, mat4_t model_mat4)
{
	mat4_t translation_mat4 = {
		{ 1, 0, 0, app->model_info.position.x },
		{ 0, 1, 0, app->model_info.position.y },
		{ 0, 0, 1, app->model_info.position.z },
		{ 0, 0, 0, 1 },
	};

	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(app->model_info.yaw));

	mat4_multiplication(translation_mat4, yaw_mat4, model_mat4);
}

void set_view_mat4(app_t *app, mat4_t view_mat4)
{
	mat4_t translation_mat4 = {
		{ 1, 0, 0, -app->camera.position.x },
		{ 0, 1, 0, -app->camera.position.y },
		{ 0, 0, 1, -app->camera.position.z },
		{ 0, 0, 0, 1 },
	};
	mat4_t yaw_mat4;
	set_yaw_mat4(yaw_mat4, radians(-app->camera.rotation.y));
	mat4_t pitch_mat4;
	set_pitch_mat4(pitch_mat4, radians(-app->camera.rotation.x));

	mat4_t rotation_mat4;
	mat4_multiplication(pitch_mat4, yaw_mat4, rotation_mat4);

	mat4_multiplication(rotation_mat4, translation_mat4, view_mat4);
}

void set_projection_mat4(app_t *app, mat4_t projection_mat4)
{
	GLfloat aspect_ratio = (GLfloat)app->window.width / (GLfloat)app->window.height;
	if (app->should_use_orthographic == true) {
		set_orthographic_projection_mat4(projection_mat4,
				-app->model_info.bounding_box.max_distance * aspect_ratio * 1.1,
				app->model_info.bounding_box.max_distance * aspect_ratio * 1.1,
				-app->model_info.bounding_box.max_distance * 1.1,
				app->model_info.bounding_box.max_distance * 1.1, NEAR_PLANE, FAR_PLANE);
	} else {
		set_perspective_projection_mat4(projection_mat4, radians(app->fov),
				aspect_ratio, NEAR_PLANE, FAR_PLANE);
	}
}

void set_view_model_mat4(app_t *app, mat4_t view_model_mat4)
{
	mat4_t model_mat4;
	set_model_mat4(app, model_mat4);

	mat4_t view_mat4;
	set_view_mat4(app, view_mat4);

	mat4_multiplication(view_mat4, model_mat4, view_model_mat4);
}

void set_projection_view_mat4(app_t *app, mat4_t projection_view_mat4)
{
	mat4_t view_mat4;
	set_view_mat4(app, view_mat4);

	mat4_t projection_mat4;
	set_projection_mat4(app, projection_mat4);

	mat4_multiplication(projection_mat4, view_mat4, projection_view_mat4);
}

void set_projection_view_model_mat4(app_t *app, mat4_t projection_view_model_mat4)
{
	mat4_t view_model_mat4;
	set_view_model_mat4(app, view_model_mat4);

	mat4_t projection_mat4;
	set_projection_mat4(app, projection_mat4);

	mat4_multiplication(projection_mat4, view_model_mat4, projection_view_model_mat4);
}

