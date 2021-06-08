#pragma once

#define GLFW_INCLUDE_NONE

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "language.h"
#include <GLFW/glfw3.h>

void InputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void InitInput(void);
void UpdateInput(void);

b32 KeyPressed(i32 key);
b32 KeyReleased(i32 key);
b32 KeyDown(i32 key);
