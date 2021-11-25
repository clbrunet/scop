#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/utils.h"

static GLuint create_shader(GLenum shader_type, const char *path)
{
	GLchar *shader_src = get_file_content(path, NULL);
	if (shader_src == NULL) {
		return 0;
	}
	GLuint shader = glCreateShader(shader_type);
	assert(shader != 0);
	glShaderSource(shader, 1, (const GLchar *const *)&shader_src, NULL);
	free(shader_src);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char info_log[512];
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		fprintf(stderr, "Vertex shader compilation error\n%s\n", info_log);
		return 0;
	}
	return shader;
}

GLuint create_program(const char *vertex_shader_path, const char *fragment_shader_path)
{
	GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_path);
	GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_shader_path);
	if (vertex_shader == 0 || fragment_shader == 0) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return 0;
	}

	GLuint program = glCreateProgram();
	assert(program != 0);
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char info_log[512];
		glGetProgramInfoLog(program, 512, NULL, info_log);
		fprintf(stderr, "Program link error\n%s\n", info_log);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(program);
		return 0;
	}
	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return program;
}
