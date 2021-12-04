#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scop/utils/file.h"

// @todo color map support
// @todo order of bytes
// @todo refactor
unsigned char *load_tga(const char *path, int *width, int *height, int *channel_count)
{
	size_t file_length;
	unsigned char *file_content = (unsigned char *)read_file(path, &file_length);
	if (file_content == NULL) {
		return NULL;
	}
	if (file_length < 18) {
		fprintf(stderr, "Image '%s' isn't in a valid Truevision TGA format.\n", path);
		free(file_content);
		return NULL;
	}
	unsigned char id_length = file_content[0];
	unsigned char image_type = file_content[2];
	if (image_type != 2) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only true color image.\n", path);
		free(file_content);
		return NULL;
	}
	unsigned short color_map_length = file_content[5] | file_content[6] << 8;
	unsigned char color_map_entry_size = file_content[7] | file_content[8] << 8;
	*width = file_content[12] | file_content[13] << 8;
	*height = file_content[14] | file_content[15] << 8;
	unsigned char pixel_depth = file_content[16];
	if (pixel_depth != 24 && pixel_depth != 32) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only 24 and 42 pixel depth.\n", path);
		free(file_content);
		return NULL;
	}
	*channel_count = pixel_depth / 8;
	unsigned int data_offset = 18 + id_length + color_map_length * (color_map_entry_size / 8);
	size_t data_length = *width * *height * *channel_count;
	if (file_length != data_offset + data_length) {
		fprintf(stderr, "Image '%s' isn't in a valid Truevision TGA format.\n", path);
		free(file_content);
		return NULL;
	}
	unsigned char *data = malloc(data_length * sizeof(unsigned char));
	if (data == NULL) {
		free(file_content);
		return NULL;
	}
	memcpy(data, file_content + data_offset, data_length);
	free(file_content);
	return data;
}
