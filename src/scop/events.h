#ifndef SCOP_EVENTS_H
#define SCOP_EVENTS_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

#endif
