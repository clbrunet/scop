#ifndef SCOP_LOAD_TGA_H
#define SCOP_LOAD_TGA_H

#include <sys/types.h>

u_char *load_tga(const char *path, int *width, int *height, int *channel_count);

#endif
