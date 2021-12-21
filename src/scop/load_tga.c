#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "scop/utils/file.h"

static size_t parse_tga_headers(const char *path, const u_char *file_content, size_t file_size,
		int *width, int *height, int *channel_count, bool *is_top_to_bottom)
{
	if (file_size < 18) {
		fprintf(stderr, "Image '%s' isn't in a valid Truevision TGA format.\n", path);
		return (size_t)-1;
	}
	u_char id_length = file_content[0];
	u_char color_map_type = file_content[1];
	if (color_map_type != 0) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only unmapped image.\n", path);
		return (size_t)-1;
	}
	u_char image_type = file_content[2];
	if (image_type != 2) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only uncompressed true color image.\n", path);
		return (size_t)-1;
	}
	*width = file_content[12] | file_content[13] << 8;
	*height = file_content[14] | file_content[15] << 8;
	u_char pixel_depth = file_content[16];
	if (pixel_depth != 24 && pixel_depth != 32) {
		fprintf(stderr, "Error while loading '%s'.\n"
				"Support only 24 and 32 pixel depth.\n", path);
		return (size_t)-1;
	}
	*channel_count = pixel_depth / 8;
	if (file_content[17] & 0b00100000) {
		*is_top_to_bottom = true;
	} else {
		*is_top_to_bottom = false;
	}
	return 18 + id_length;
}

u_char *load_tga(const char *path, int *width, int *height, int *channel_count)
{
	const char *extension =  strrchr(path, '.');
	if (extension == NULL || strcmp(extension, ".tga") != 0) {
		fprintf(stderr, "Image '%s' must have '.tga' extension.\n", path);
		return NULL;
	}
	size_t file_size;
	u_char *file_content = (u_char *)read_file(path, &file_size);
	if (file_content == NULL) {
		return NULL;
	}
	bool is_top_to_bottom;
	size_t file_data_offset = parse_tga_headers(path, file_content, file_size,
			width, height, channel_count, &is_top_to_bottom);
	if (file_data_offset == (size_t)-1) {
		free(file_content);
		return NULL;
	}
	size_t width_size = *width * *channel_count;
	size_t file_data_size = width_size * *height;
	size_t padding_size;
	if (*width % 4 != 0) {
		padding_size = *width % 4;
	} else {
		padding_size = 0;
	}
	u_char *data = malloc((file_data_size + (padding_size * *height))* sizeof(u_char));
	if (data == NULL) {
		free(file_content);
		return NULL;
	}
	u_char *file_data_it = file_content + file_data_offset;
	if (is_top_to_bottom == true) {
		data += file_data_size + (padding_size * *height);
		for (int i = 0; i < *height; i++) {
			memcpy(data - padding_size, "\0\0\0\0", padding_size);
			data -= width_size + padding_size;
			memcpy(data, file_data_it, width_size);
			file_data_it += width_size;
		}
	} else {
		u_char *data_it = data;
		for (int i = 0; i < *height; i++) {
			memcpy(data_it, file_data_it, width_size);
			memcpy(data_it + width_size, "\0\0\0\0", padding_size);
			data_it += width_size + padding_size;
			file_data_it += width_size;
		}
	}
	free(file_content);
	return data;
}
