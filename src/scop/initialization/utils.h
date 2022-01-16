#ifndef SCOP_INITIALIZATION_UTILS_H
#define SCOP_INITIALIZATION_UTILS_H

#include "scop/app.h"
#include "scop/initialization.h"

int initialize_glfw(app_t *app);
int initialize_gl(app_t *app);

int initialize_model_vao(app_t *app, model_t *model, texture_t *texture);
int initialize_light_vao(app_t *app);

#endif
