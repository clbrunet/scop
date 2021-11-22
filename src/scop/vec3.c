#include <stdio.h>

#include "scop/vec3.h"

void print_vec3(vec3_t vec3)
{
	printf("x: %f\n", vec3.x);
	printf("y: %f\n", vec3.y);
	printf("z: %f\n", vec3.z);
	printf("\n");
}
