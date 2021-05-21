#pragma once

#include "language.hpp"
#include <array>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Input
{
  public:
    Input(void);

    // call before polling events
    void Update(void);

    b32 KeyPressed(i32 key);
    b32 KeyReleased(i32 key);
    b32 KeyDown(i32 key);

    friend void Input_KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

  private:
    std::array<b8, GLFW_KEY_LAST> m_KeysDown;
    std::array<b8, GLFW_KEY_LAST> m_KeysDownPrev;
};

