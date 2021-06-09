#pragma once

#define GLFW_INCLUDE_NONE

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "language.h"
#include "geometry.h"
#include <GLFW/glfw3.h>

void KeyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void CursorPositionCallback(GLFWwindow* window, double x, double y);

void InitInput(void);
void UpdateInput(void);

i32 *GetKeyStack(i32 *n);
b32 IsKeyCharacter(i32 key);

b32 KeyPressed(i32 key);
b32 KeyReleased(i32 key);
b32 KeyDown(i32 key);

b32 MouseClicked(i32 button);
b32 MouseReleased(i32 button);
b32 MouseDown(i32 button);
v2 MousePos(void);
