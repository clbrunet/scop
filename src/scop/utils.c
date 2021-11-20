#include <math.h>

float radians(float degrees)
{
	return degrees * (M_PI / 180);
}

float degrees(float radians)
{
	return radians * (180 / M_PI);
}
