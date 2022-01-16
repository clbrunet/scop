#ifndef SCOP_LOAD_OBJ_UTILS_H
#define SCOP_LOAD_OBJ_UTILS_H

#include "scop/initialization.h"
#include "scop/load_obj.h"

int fill_model_datas(model_t *model, polygon_t *polygons, char **lines);
int count_model_datas(model_t *model, GLuint *polygons_count, char **lines);

face_format_t get_face_format(char *data);

void set_bounding_box_axes(model_t *model);
void center_bounding_box(model_t *model);

#endif
