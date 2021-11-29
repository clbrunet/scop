#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>

char *read_file(const char *path, size_t *file_length_ptr)
{
	assert(path != NULL);

	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		err(1, "%s", path);
		return NULL;
	}

	if (fseek(file, 0, SEEK_END) == -1) {
		err(1, "fseek");
		fclose(file);
		return NULL;
	}
	long file_length = ftell(file);
	if (file_length == -1) {
		err(1, "ftell");
		fclose(file);
		return NULL;
	}
	if (file_length_ptr) {
		*file_length_ptr = file_length;
	}

	char *content = (char *)malloc((file_length + 1) * sizeof(char));
	if (content == NULL) {
		err(1, "malloc");
		fclose(file);
		return NULL;
	}

	if (fseek(file, 0, SEEK_SET) == -1) {
		err(1, "fseek");
		free(content);
		fclose(file);
		return NULL;
	}
	clearerr(file);
	fread(content, sizeof(char), file_length, file);
	if (ferror(file)) {
		err(1, "fread %s", path);
		free(content);
		fclose(file);
		return NULL;
	}
	content[file_length] = '\0';

	fclose(file);
	return content;
}

