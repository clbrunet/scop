#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "glad/glad.h"

GLfloat radians(GLfloat degrees)
{
	assert(!isnanf(degrees) && !isinff(degrees));

	return degrees * (M_PI / 180);
}

GLfloat degrees(GLfloat radians)
{
	assert(!isnanf(radians) && !isinff(radians));

	return radians * (180 / M_PI);
}
