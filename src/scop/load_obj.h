#ifndef SCOP_LOAD_OBJ_H
#define SCOP_LOAD_OBJ_H

typedef struct model_s model_t;

typedef enum face_format_e {
	V,
	V_VT,
	V_VN,
	V_VT_VN,
}	face_format_t;

int load_obj(model_t *model, const char *path);

#endif
