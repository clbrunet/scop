#include "glad/glad.h"

#include "scop/app.h"

void destruction(app_t *app)
{
	glDeleteVertexArrays(1, &app->opengl.model_vao.id);
	glDeleteBuffers(1, &app->opengl.model_vao.vertex_buffer);
	glDeleteTextures(1, &app->opengl.model_vao.texture_map);

	glDeleteVertexArrays(1, &app->opengl.light_vao.id);
	glDeleteBuffers(1, &app->opengl.light_vao.vertex_buffer);

	glDeleteProgram(app->opengl.model_program.id);
	glDeleteProgram(app->opengl.normals_program.id);
	glDeleteProgram(app->opengl.model_lighting_program.id);
	glDeleteProgram(app->opengl.light_program.id);

	glfwTerminate();
}
