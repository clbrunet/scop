#ifndef SCOP_UTILS_H
#define SCOP_UTILS_H

#include <stddef.h>

char *get_file_content(const char *path, size_t *file_length);

float radians(float degrees);
float degrees(float radians);

#endif
