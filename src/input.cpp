#include "input.hpp"
#include "app.hpp"

Input::Input(void)
{
    m_KeysDown.fill(false);
    m_KeysDownPrev.fill(false);
}

void Input::Update(void)
{
    m_KeysDownPrev = m_KeysDown;
}

b32 Input::KeyPressed(i32 key)
{
    return !m_KeysDownPrev[key] && m_KeysDown[key];
}

b32 Input::KeyReleased(i32 key)
{
    return m_KeysDownPrev[key] && !m_KeysDown[key];
}

b32 Input::KeyDown(i32 key)
{
    return m_KeysDown[key];
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
        app.input.m_KeysDown[key] = true;
        break;
    case GLFW_RELEASE:
        app.input.m_KeysDown[key] = false;
        break;
    case GLFW_REPEAT:
    default:
        break;
    }
}
