#include "input.h"
#include <string.h>

static struct
{
    b8 keysDown[GLFW_KEY_LAST];
    b8 prevKeysDown[GLFW_KEY_LAST];
} g_Input;

void InputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
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

void InitInput(void)
{
    memset(g_Input.keysDown, 0, sizeof(g_Input.keysDown));
    memset(g_Input.prevKeysDown, 0, sizeof(g_Input.prevKeysDown));
}

void UpdateInput(void)
{
    memcpy(g_Input.prevKeysDown, g_Input.keysDown, sizeof(g_Input.keysDown));
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
