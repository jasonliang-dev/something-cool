#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef struct Window Window;
struct Window
{
    GLFWwindow *handle;
} g_Window;

void InitWindow(void);
