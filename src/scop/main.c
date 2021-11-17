#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

static void print_key(int key, int scancode, int action, int mods)
{
	return;
	printf("\n");
	printf("key: %i\n", key);
	printf("scancode: %i\n", scancode);
	printf("action: %i\n", action);
	printf("mods: %i\n", mods);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	print_key(key, scancode, action, mods);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		else if (key == GLFW_KEY_L) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (key == GLFW_KEY_F) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (key == GLFW_KEY_P) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
	}

}

static GLFWwindow *initialize_glfw()
{
	GLFWwindow *window = NULL;
	if (glfwInit() == GLFW_FALSE) {
		fprintf(stderr, "GLFW initialization failed\n");
		return NULL;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "SCOP", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Window or OpenGL context creation failed failed\n");
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);
	glfwSetKeyCallback(window, &key_callback);
	return window;
}

static int initialize_gl()
{
	if (gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress) == 0) {
		fprintf(stderr, "Glad initialization failed\n");
		return -1;
	}

	glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);
	return 0;
}

char *get_file_content(const char *path)
{
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	char *content = malloc((file_size + 1) * sizeof(char));
	if (content == NULL) {
		fclose(file);
		return NULL;
	}
	fseek(file, 0, SEEK_SET);
	fread(content, sizeof(char), file_size, file);
	content[file_size] = '\0';
	fclose(file);
	return content;
}

static GLuint create_shader(GLenum shader_type, const char *path)
{
	GLchar *shader_src = get_file_content(path);
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

static GLuint create_program()
{
	GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, "./shaders/vertex.glsl");
	GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, "./shaders/fragment.glsl");
	if (vertex_shader == 0 || fragment_shader == 0) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glfwTerminate();
		return 0;
	}

	GLuint program = glCreateProgram();
	assert(program != 0);
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char info_log[512];
		glGetProgramInfoLog(program, 512, NULL, info_log);
		fprintf(stderr, "Program link error\n%s\n", info_log);
		return 0;
	}
	return program;
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	GLFWwindow *window = initialize_glfw();
	if (window == NULL || initialize_gl() == -1) {
		glfwTerminate();
		return 1;
	}

	GLuint program = create_program();
	if (program == 0) {
		glfwTerminate();
		return 2;
	}
	glUseProgram(program);

	int vertices_count = 4;
	GLfloat vertices[4 * 3] = {
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};
	int vertices_dimension = 3;

	int indices_count = 2;
	GLuint indices[2 * 3] = {
		0, 1, 2,
		2, 3, 0
	};
	int indices_dimension = 3;

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	GLuint element_buffer;
	glGenBuffers(1, &element_buffer);

	glBindVertexArray(vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * vertices_dimension * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * indices_dimension * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, vertices_dimension, GL_FLOAT, GL_FALSE, vertices_dimension * sizeof(GLfloat), (const GLvoid *)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)(0 * sizeof(GLuint)));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}
