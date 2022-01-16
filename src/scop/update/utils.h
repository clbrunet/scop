#ifndef SCOP_UPDATE_UTILS_H
#define SCOP_UPDATE_UTILS_H

#include "scop/app.h"
#include "scop/matrices/mat4.h"

void process_inputs(app_t *app);

void set_model_mat4(app_t *app, mat4_t model_mat4);
void set_view_mat4(app_t *app, mat4_t view_mat4);
void set_projection_mat4(app_t *app, mat4_t projection_mat4);
void set_view_model_mat4(app_t *app, mat4_t view_model_mat4);
void set_projection_view_mat4(app_t *app, mat4_t projection_view_mat4);
void set_projection_view_model_mat4(app_t *app, mat4_t projection_view_model_mat4);

#endif
