#pragma once

#define GLFW_INCLUDE_NONE

#include "language.h"
#include <GLFW/glfw3.h>

void InputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void InitInput(void);
void UpdateInput(void);

b32 KeyPressed(i32 key);
b32 KeyReleased(i32 key);
b32 KeyDown(i32 key);
