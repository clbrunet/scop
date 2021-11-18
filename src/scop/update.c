#include <math.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "scop/app.h"

void update(app_t *app, double time, double delta_time)
{
	(void)delta_time;
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat x_displacement = sin(time) / 2;
	glUniform1f(app->x_displacement_uniform_location, x_displacement);

	GLfloat blue = cos(time) / 4 + 0.75;
	glUniform4f(app->color_uniform_location, 0, 1, blue, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)(0 * sizeof(GLuint)));
}

