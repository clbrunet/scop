#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <err.h>

void free_strs(char **strs)
{
	char **iter = strs;
	while (*iter != NULL) {
		free(*iter);
		iter++;
	}
	free(strs);
}

static size_t split_count_tokens(const char *str, char delim)
{
	size_t count = 0;

	while (*str != '\0') {
		if (*str != delim) {
			char *delim_pos = strchr(str, delim);
			count++;
			if (delim_pos == NULL) {
				break;
			} else {
				str = delim_pos + 1;
			}
		} else {
			str++;
		}
	}
	return count;
}

char **split(const char *str, char delim)
{
	assert(str != NULL);

	char **strs = malloc((split_count_tokens(str, delim) + 1) * sizeof(char *));
	if (strs == NULL) {
		err(1, "malloc");
		return NULL;
	}

	char **iter = strs;
	while (*str != '\0') {
		if (*str != delim) {
			const char *token_start = str;
			while (*str != '\0' && *str != delim) {
				str++;
			}
			*iter = strndup(token_start, str - token_start);
			if (*iter == NULL) {
				err(1, "strndup");
				free_strs(strs);
				return NULL;
			}
			iter++;
		} else {
			str++;
		}
	}
	*iter = NULL;
	return strs;
}
