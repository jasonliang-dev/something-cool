#include "input.hpp"

Input g_Input;

Input::Input(void)
{
    m_KeysDown.fill(false);
    m_KeysDownPrev.fill(false);
}

void Input::Update(void)
{
    m_KeysDownPrev = m_KeysDown;
}

void Input_KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)window;
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_UNKNOWN)
    {
        return;
    }

    switch (action)
    {
    case GLFW_PRESS:
        g_Input.m_KeysDown[key] = true;
        break;
    case GLFW_RELEASE:
        g_Input.m_KeysDown[key] = false;
        break;
    case GLFW_REPEAT:
    default:
        break;
    }
}

b32 KeyPressed(i32 key)
{
    return !g_Input.m_KeysDownPrev[key] && g_Input.m_KeysDown[key];
}

b32 KeyReleased(i32 key)
{
    return g_Input.m_KeysDownPrev[key] && !g_Input.m_KeysDown[key];
}

b32 KeyDown(i32 key)
{
    return g_Input.m_KeysDown[key];
}
