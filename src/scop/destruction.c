#include "glad/glad.h"

#include "scop/app.h"

void destruction(app_t *app)
{
	glDeleteVertexArrays(1, &app->vertex_array);
	glDeleteBuffers(1, &app->vertex_buffer);
	glDeleteTextures(1, &app->texture_map);

	glDeleteProgram(app->program);

	glfwTerminate();
}
