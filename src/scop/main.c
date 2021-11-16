#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h> 

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// printf("\n");
	// printf("key: %i\n", key);
	// printf("scancode: %i\n", scancode);
	// printf("action: %i\n", action);
	// printf("mods: %i\n", mods);
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

GLFWwindow *initialize_glfw()
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
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);
	glfwSetKeyCallback(window, &key_callback);
	return window;
}

int initialize_gl()
{
	if (gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress) == 0) {
		fprintf(stderr, "Glad initialization failed\n");
		return -1;
	}

	glViewport(0, 0, INIT_WIDTH, INIT_HEIGHT);
	return 0;
}

char *get_file_content(char *path)
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

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	GLFWwindow *window = initialize_glfw();
	if (window == NULL) {
		return 1;
	}
	if (initialize_gl() == -1) {
		return 2;
	}

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
	int vertices_count = 3;
	int vertices_dimension = 3;

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * vertices_dimension * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, vertices_dimension, GL_FLOAT, GL_FALSE, vertices_dimension * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLchar *vertex_shader_src = get_file_content("./shaders/vertex.glsl");
	if (vertex_shader_src == NULL) {
		glfwTerminate();
		return 3;
	}
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const GLchar *const *)&vertex_shader_src, NULL);
	free(vertex_shader_src);
	glCompileShader(vertex_shader);
	GLint success;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char info_log[512];
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		fprintf(stderr, "Vertex shader compilation error\n%s\n", info_log);
		glfwTerminate();
		return 4;
	}

	GLchar *fragment_shader_src = get_file_content("./shaders/fragment.glsl");
	if (fragment_shader_src == NULL) {
		glfwTerminate();
		return 5;
	}
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const GLchar *const *)&fragment_shader_src, NULL);
	free(fragment_shader_src);
	glCompileShader(fragment_shader);
	// GLint success;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char info_log[512];
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		fprintf(stderr, "Fragment shader compilation error\n%s\n", info_log);
		glfwTerminate();
		return 6;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char info_log[512];
		glGetProgramInfoLog(program, 512, NULL, info_log);
		fprintf(stderr, "Program link error\n%s\n", info_log);
		glfwTerminate();
		return 7;
	}
	glUseProgram(program);

	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}
