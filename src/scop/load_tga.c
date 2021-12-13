#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "scop/utils/file.h"

// @todo color map support
// @todo order of bytes
// @todo refactor
u_char *load_tga(const char *path, int *width, int *height, int *channel_count)
{
	size_t file_length;
	u_char *file_content = (u_char *)read_file(path, &file_length);
	if (file_content == NULL) {
		return NULL;
	}
	if (file_length < 18) {
		fprintf(stderr, "Image '%s' isn't in a valid Truevision TGA format.\n", path);
		free(file_content);
		return NULL;
	}
	u_char id_length = file_content[0];
	u_char image_type = file_content[2];
	if (image_type != 2) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only true color image.\n", path);
		free(file_content);
		return NULL;
	}
	u_short color_map_length = file_content[5] | file_content[6] << 8;
	u_char color_map_entry_size = file_content[7] | file_content[8] << 8;
	*width = file_content[12] | file_content[13] << 8;
	*height = file_content[14] | file_content[15] << 8;
	u_char pixel_depth = file_content[16];
	if (pixel_depth != 24 && pixel_depth != 32) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only 24 and 42 pixel depth.\n", path);
		free(file_content);
		return NULL;
	}
	*channel_count = pixel_depth / 8;
	size_t data_offset = 18 + id_length + color_map_length * (color_map_entry_size / 8);
	size_t data_length = *width * *height * *channel_count;
	if (file_length != data_offset + data_length) {
		fprintf(stderr, "Image '%s' isn't in a valid Truevision TGA format.\n", path);
		free(file_content);
		return NULL;
	}
	size_t padding_length;
	if (*width % 4 != 0) {
		padding_length = (4 - *width % 4);
	} else {
		padding_length = 0;
	}
	u_char *data = malloc((data_length + padding_length * *height)* sizeof(u_char));
	if (data == NULL) {
		free(file_content);
		return NULL;
	}
	if (padding_length != 0) {
		size_t offset = 0;
		for (int i = 0; i < *height; i++) {
			memcpy(data + offset + (i * padding_length), file_content + data_offset + offset, *width * *channel_count);
			offset += *width * *channel_count;
			memcpy(data + offset + (i * padding_length), "\0\0\0\0", padding_length);
		}
	} else {
		memcpy(data, file_content + data_offset, data_length);
	}
	free(file_content);
	return data;
}
