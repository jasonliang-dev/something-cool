#include "input.h"
#include <assert.h>
#include <string.h>

#define KEY_STACK_MAX 32

static struct
{
    i32 keyStack[KEY_STACK_MAX];
    i32 keyStackCount;

    b8 keysDown[GLFW_KEY_LAST];
    b8 prevKeysDown[GLFW_KEY_LAST];
    b8 mouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
    b8 prevMouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
    v2 mousePos;
} g_Input;

static void AddToKeyStack(int key)
{
    assert(g_Input.keyStackCount < KEY_STACK_MAX);
    g_Input.keyStack[g_Input.keyStackCount++] = key;
}

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
        AddToKeyStack(key);
        g_Input.keysDown[key] = true;
        break;
    case GLFW_RELEASE:
        g_Input.keysDown[key] = false;
        break;
    case GLFW_REPEAT:
        AddToKeyStack(key);
        break;
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
    g_Input.keyStackCount = 0;

    memcpy(g_Input.prevKeysDown, g_Input.keysDown, sizeof(g_Input.keysDown));
    memcpy(g_Input.prevMouseButtonsDown, g_Input.mouseButtonsDown,
           sizeof(g_Input.mouseButtonsDown));
}

KeyStack GetKeyStack(void)
{
    return (KeyStack){
        .begin = g_Input.keyStack,
        .end = g_Input.keyStack + g_Input.keyStackCount,
    };
}

b32 IsKeyCharacter(i32 key)
{
    return GLFW_KEY_SPACE <= key && key <= GLFW_KEY_GRAVE_ACCENT;
}

b32 KeyPressed(i32 key)
{
    static_assert(GLFW_KEY_SPACE == ' ' && GLFW_KEY_0 == '0' && GLFW_KEY_A == 'A',
                  "Expected GLFW keys map to ASCII");

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
