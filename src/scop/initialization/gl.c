#include <stdio.h>
#include <assert.h>

#include "scop/initialization.h"
#include "scop/create_program.h"

static int initialize_gl_model_program(model_program_t *model_program)
{
	model_program->id = create_program("./shaders/model.vert", NULL,
			"./shaders/model.frag");
	if (model_program->id == 0) {
		return -1;
	}

	model_program->projection_view_model
		= glGetUniformLocation(model_program->id, "projection_view_model");
	assert(model_program->projection_view_model != -1);
	model_program->model_color
		= glGetUniformLocation(model_program->id, "model_color");
	assert(model_program->model_color != -1);
	model_program->texture_portion
		= glGetUniformLocation(model_program->id, "texture_portion");
	assert(model_program->texture_portion != -1);
	return 0;
}

static int initialize_gl_normals_program(normals_program_t *normals_program)
{
	normals_program->id = create_program("./shaders/normals.vert", "./shaders/normals.geom",
			"./shaders/normals.frag");
	if (normals_program->id == 0) {
		return -1;
	}

	normals_program->view_model
		= glGetUniformLocation(normals_program->id, "view_model");
	assert(normals_program->view_model != -1);
	normals_program->projection
		= glGetUniformLocation(normals_program->id, "projection");
	assert(normals_program->projection != -1);
	return 0;
}

static int initialize_gl_model_lighting_program(app_t *app, model_lighting_program_t *model_lighting_program)
{
	model_lighting_program->id = create_program("./shaders/model_lighting.vert", NULL,
			"./shaders/model_lighting.frag");
	if (model_lighting_program->id == 0) {
		return -1;
	}

	model_lighting_program->view_model
		= glGetUniformLocation(model_lighting_program->id, "view_model");
	assert(model_lighting_program->view_model != -1);
	model_lighting_program->projection_view_model
		= glGetUniformLocation(model_lighting_program->id, "projection_view_model");
	assert(model_lighting_program->projection_view_model != -1);
	model_lighting_program->material_diffuse
		= glGetUniformLocation(model_lighting_program->id, "material.diffuse");
	assert(model_lighting_program->material_diffuse != -1);
	model_lighting_program->light_position
		= glGetUniformLocation(model_lighting_program->id, "light.position");
	assert(model_lighting_program->light_position != -1);
	model_lighting_program->texture_portion
		= glGetUniformLocation(model_lighting_program->id, "texture_portion");
	assert(model_lighting_program->texture_portion != -1);

	glUseProgram(model_lighting_program->id);
	GLenum error = glGetError();
	if (error == GL_INVALID_OPERATION) {
		fprintf(stderr, "glUseProgram invalid operation error\n");
		glDeleteProgram(model_lighting_program->id);
		return -1;
	}
	assert(error == GL_NO_ERROR);

	GLint material_shininess = glGetUniformLocation(model_lighting_program->id,
			"material.shininess");
	assert(material_shininess != -1);
	glUniform1f(material_shininess, 64);
	assert(glGetError() == GL_NO_ERROR);
	GLint light_ambient = glGetUniformLocation(model_lighting_program->id,
			"light.ambient");
	assert(light_ambient != -1);
	glUniform3fv(light_ambient, 1, app->light.ambient.array);
	assert(glGetError() == GL_NO_ERROR);
	GLint light_diffuse = glGetUniformLocation(model_lighting_program->id,
			"light.diffuse");
	assert(light_diffuse != -1);
	glUniform3fv(light_diffuse, 1, app->light.diffuse.array);
	assert(glGetError() == GL_NO_ERROR);
	GLint light_specular = glGetUniformLocation(model_lighting_program->id,
			"light.specular");
	assert(light_specular != -1);
	glUniform3fv(light_specular, 1, app->light.specular.array);
	assert(glGetError() == GL_NO_ERROR);

	return 0;
}

static int initialize_gl_light_program(light_program_t *light_program)
{
	light_program->id = create_program("./shaders/light.vert", NULL,
			"./shaders/light.frag");
	if (light_program->id == 0) {
		return -1;
	}

	light_program->projection_view
		= glGetUniformLocation(light_program->id, "projection_view");
	assert(light_program->projection_view != -1);
	return 0;
}

int initialize_gl(app_t *app)
{
	if (gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress) == 0) {
		fprintf(stderr, "Glad initialization failed\n");
		return -1;
	}

	const GLubyte *version = glGetString(GL_VERSION);
	assert(version != NULL);
	printf("OpenGL version : %s\n", version);

	glViewport(0, 0, app->window.width, app->window.height);
	assert(glGetError() == GL_NO_ERROR);
	glEnable(GL_DEPTH_TEST);
	assert(glGetError() == GL_NO_ERROR);
	glPointSize(3);
	assert(glGetError() == GL_NO_ERROR);
	glEnable(GL_PROGRAM_POINT_SIZE);
	assert(glGetError() == GL_NO_ERROR);

	if (initialize_gl_model_program(&app->opengl.model_program) == -1) {
		return -1;
	}
	if (initialize_gl_normals_program(&app->opengl.normals_program) == -1) {
		glDeleteProgram(app->opengl.model_program.id);
		return -1;
	}
	if (initialize_gl_model_lighting_program(app, &app->opengl.model_lighting_program) == -1) {
		glDeleteProgram(app->opengl.model_program.id);
		glDeleteProgram(app->opengl.normals_program.id);
		return -1;
	}
	if (initialize_gl_light_program(&app->opengl.light_program) == -1) {
		glDeleteProgram(app->opengl.model_program.id);
		glDeleteProgram(app->opengl.normals_program.id);
		glDeleteProgram(app->opengl.model_lighting_program.id);
		return -1;
	}

	glGenVertexArrays(1, &app->opengl.model_vao.id);
	assert(glGetError() == GL_NO_ERROR);
	glGenBuffers(1, &app->opengl.model_vao.vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);
	glGenTextures(1, &app->opengl.model_vao.texture_map);
	assert(glGetError() == GL_NO_ERROR);

	glGenVertexArrays(1, &app->opengl.light_vao.id);
	assert(glGetError() == GL_NO_ERROR);
	glGenBuffers(1, &app->opengl.light_vao.vertex_buffer);
	assert(glGetError() == GL_NO_ERROR);

	return 0;
}
