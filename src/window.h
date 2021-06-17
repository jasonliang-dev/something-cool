#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef struct Window Window;
struct Window
{
    GLFWwindow *handle;
};

extern Window g_Window;

void InitWindow(i32 width, i32 height);
