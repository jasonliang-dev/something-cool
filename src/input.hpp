#pragma once

#define GLFW_INCLUDE_NONE

#include "language.hpp"
#include <GLFW/glfw3.h>
#include <array>

struct Input
{
    std::array<b8, GLFW_KEY_LAST> m_KeysDown;
    std::array<b8, GLFW_KEY_LAST> m_KeysDownPrev;

    Input(void);

    // call before polling events
    void Update(void);
};

extern Input g_Input;

void Input_KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

b32 KeyPressed(i32 key);
b32 KeyReleased(i32 key);
b32 KeyDown(i32 key);
