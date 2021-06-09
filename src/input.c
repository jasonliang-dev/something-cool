#include "input.h"
#include <string.h>

static struct
{
    b8 keysDown[GLFW_KEY_LAST];
    b8 prevKeysDown[GLFW_KEY_LAST];
    b8 mouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
    b8 prevMouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
    v2 mousePos;
} g_Input;

void KeyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
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
        g_Input.keysDown[key] = true;
        break;
    case GLFW_RELEASE:
        g_Input.keysDown[key] = false;
        break;
    case GLFW_REPEAT:
    default:
        break;
    }
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    (void)window;
    (void)button;
    (void)mods;

    switch (action)
    {
    case GLFW_PRESS:
        g_Input.mouseButtonsDown[button] = true;
        break;
    case GLFW_RELEASE:
        g_Input.mouseButtonsDown[button] = false;
        break;
    default:
        break;
    }
}

void CursorPositionCallback(GLFWwindow *window, double x, double y)
{
    (void)window;
    g_Input.mousePos = v2((f32)x, (f32)y);
}

void InitInput(void)
{
    memset(&g_Input, 0, sizeof(g_Input));
}

void UpdateInput(void)
{
    memcpy(g_Input.prevKeysDown, g_Input.keysDown, sizeof(g_Input.keysDown));
    memcpy(g_Input.prevMouseButtonsDown, g_Input.mouseButtonsDown,
           sizeof(g_Input.mouseButtonsDown));
}

b32 KeyPressed(i32 key)
{
    return !g_Input.prevKeysDown[key] && g_Input.keysDown[key];
}

b32 KeyReleased(i32 key)
{
    return g_Input.prevKeysDown[key] && !g_Input.keysDown[key];
}

b32 KeyDown(i32 key)
{
    return g_Input.keysDown[key];
}

b32 MouseClicked(i32 button)
{
    return !g_Input.prevMouseButtonsDown[button] && g_Input.mouseButtonsDown[button];
}

b32 MouseReleased(i32 button)
{
    return g_Input.prevMouseButtonsDown[button] && !g_Input.mouseButtonsDown[button];
}

b32 MouseDown(i32 button)
{
    return g_Input.mouseButtonsDown[button];
}

v2 MousePos(void)
{
    return g_Input.mousePos;
}
