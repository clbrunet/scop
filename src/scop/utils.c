#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char *get_file_content(const char *path, size_t *file_length)
{
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		return NULL;
	}
	if (fseek(file, 0, SEEK_END) == -1) {
		fclose(file);
		return NULL;
	}
	long file_size = ftell(file);
	if (file_size == -1) {
		fclose(file);
		return NULL;
	}
	char *content = (char *)malloc((file_size + 1) * sizeof(char));
	if (content == NULL) {
		fclose(file);
		return NULL;
	}
	if (file_size == 0) {
		content[0] = '\0';
		fclose(file);
		return content;
	}
	if (fseek(file, 0, SEEK_SET) == -1) {
		fclose(file);
		free(content);
		return NULL;
	}
	if (fread(content, sizeof(char), file_size, file) == 0) {
		fclose(file);
		free(content);
		return NULL;
	}
	content[file_size] = '\0';
	fclose(file);
	if (file_length) {
		*file_length = file_size;
	}
	return content;
}

float radians(float degrees)
{
	return degrees * (M_PI / 180);
}

float degrees(float radians)
{
	return radians * (180 / M_PI);
}
