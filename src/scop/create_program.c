#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/utils/file.h"

static GLuint create_shader(GLenum shader_type, const char *path)
{
	assert(path != NULL);

	GLchar *shader_source = read_file(path, NULL);
	if (shader_source == NULL) {
		return 0;
	}
	GLuint shader = glCreateShader(shader_type);
	assert(shader != 0);
	glShaderSource(shader, 1, (const GLchar *const *)&shader_source, NULL);
	assert(glGetError() == GL_NO_ERROR);
	free(shader_source);

	glCompileShader(shader);
	assert(glGetError() == GL_NO_ERROR);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	assert(glGetError() == GL_NO_ERROR);
	if (success == GL_FALSE) {
		GLint info_log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		assert(glGetError() == GL_NO_ERROR);
		GLchar *info_log = malloc(info_log_length * sizeof(GLchar));
		if (info_log == NULL) {
			err(1, "malloc");
			return 0;
		}
		glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
		assert(glGetError() == GL_NO_ERROR);
		fprintf(stderr, "Shader '%s' compilation error\n%s", path, info_log);
		return 0;
	}
	return shader;
}

static int link_program(GLuint program)
{
	glLinkProgram(program);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	assert(glGetError() == GL_NO_ERROR);
	if (success == GL_FALSE) {
		GLint info_log_length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		assert(glGetError() == GL_NO_ERROR);
		GLchar *info_log = malloc(info_log_length * sizeof(GLchar));
		if (info_log == NULL) {
			err(1, "malloc");
			return -1;
		}
		glGetProgramInfoLog(program, info_log_length, NULL, info_log);
		assert(glGetError() == GL_NO_ERROR);
		fprintf(stderr, "Program link error\n%s", info_log);
		return -1;
	}
	return 0;
}

GLuint create_program(const char *vertex_shader_path, const char *fragment_shader_path)
{
	assert(vertex_shader_path != NULL);
	assert(fragment_shader_path != NULL);

	GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_path);
	GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_shader_path);
	if (vertex_shader == 0 || fragment_shader == 0) {
		glDeleteShader(vertex_shader);
		assert(glGetError() == GL_NO_ERROR);
		glDeleteShader(fragment_shader);
		assert(glGetError() == GL_NO_ERROR);
		return 0;
	}

	GLuint program = glCreateProgram();
	assert(program != 0);
	glAttachShader(program, vertex_shader);
	assert(glGetError() == GL_NO_ERROR);
	glAttachShader(program, fragment_shader);
	assert(glGetError() == GL_NO_ERROR);

	link_program(program);

	glDetachShader(program, vertex_shader);
	assert(glGetError() == GL_NO_ERROR);
	glDetachShader(program, fragment_shader);
	assert(glGetError() == GL_NO_ERROR);
	glDeleteShader(vertex_shader);
	assert(glGetError() == GL_NO_ERROR);
	glDeleteShader(fragment_shader);
	assert(glGetError() == GL_NO_ERROR);
	return program;
}
