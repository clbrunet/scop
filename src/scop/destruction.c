#include "glad/glad.h"

#include "scop/app.h"

void destruction(app_t *app)
{
	glDeleteVertexArrays(1, &app->opengl.vertex_array);
	glDeleteBuffers(1, &app->opengl.vertex_buffer);
	glDeleteTextures(1, &app->opengl.texture_map);

	glDeleteProgram(app->opengl.model_program.id);
	glDeleteProgram(app->opengl.normals_program.id);
	glDeleteProgram(app->opengl.model_lighting_program.id);

	glfwTerminate();
}
