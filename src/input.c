#include "input.h"
#include "geometry.h"
#include <stdio.h>
#include <string.h>

static struct
{
    b8 keysDown[Key_Max];
    b8 keysDownPrev[Key_Max];
    b8 mouseButtonsDown[Mouse_Max];
    b8 mouseButtonsDownPrev[Mouse_Max];
    v2 mousePos;
    v2 mousePosPrev;
} g_Input;

const char *KeyToString(i32 key)
{
    switch (key)
    {
    case Key_Unknown:
        return "Key_Unknown";
    case Key_Esc:
        return "Key_Esc";
    case Key_F1:
        return "Key_F1";
    case Key_F2:
        return "Key_F2";
    case Key_F3:
        return "Key_F3";
    case Key_F4:
        return "Key_F4";
    case Key_F5:
        return "Key_F5";
    case Key_F6:
        return "Key_F6";
    case Key_F7:
        return "Key_F7";
    case Key_F8:
        return "Key_F8";
    case Key_F9:
        return "Key_F9";
    case Key_F10:
        return "Key_F10";
    case Key_F11:
        return "Key_F11";
    case Key_F12:
        return "Key_F12";
    case Key_F13:
        return "Key_F13";
    case Key_F14:
        return "Key_F14";
    case Key_F15:
        return "Key_F15";
    case Key_F16:
        return "Key_F16";
    case Key_F17:
        return "Key_F17";
    case Key_F18:
        return "Key_F18";
    case Key_F19:
        return "Key_F19";
    case Key_F20:
        return "Key_F20";
    case Key_Grave:
        return "Key_Grave";
    case Key_0:
        return "Key_0";
    case Key_1:
        return "Key_1";
    case Key_2:
        return "Key_2";
    case Key_3:
        return "Key_3";
    case Key_4:
        return "Key_4";
    case Key_5:
        return "Key_5";
    case Key_6:
        return "Key_6";
    case Key_7:
        return "Key_7";
    case Key_8:
        return "Key_8";
    case Key_9:
        return "Key_9";
    case Key_Minus:
        return "Key_Minus";
    case Key_Equal:
        return "Key_Equal";
    case Key_Backspace:
        return "Key_Backspace";
    case Key_Insert:
        return "Key_Insert";
    case Key_Delete:
        return "Key_Delete";
    case Key_Tab:
        return "Key_Tab";
    case Key_A:
        return "Key_A";
    case Key_B:
        return "Key_B";
    case Key_C:
        return "Key_C";
    case Key_D:
        return "Key_D";
    case Key_E:
        return "Key_E";
    case Key_F:
        return "Key_F";
    case Key_G:
        return "Key_G";
    case Key_H:
        return "Key_H";
    case Key_I:
        return "Key_I";
    case Key_J:
        return "Key_J";
    case Key_K:
        return "Key_K";
    case Key_L:
        return "Key_L";
    case Key_M:
        return "Key_M";
    case Key_N:
        return "Key_N";
    case Key_O:
        return "Key_O";
    case Key_P:
        return "Key_P";
    case Key_Q:
        return "Key_Q";
    case Key_R:
        return "Key_R";
    case Key_S:
        return "Key_S";
    case Key_T:
        return "Key_T";
    case Key_U:
        return "Key_U";
    case Key_V:
        return "Key_V";
    case Key_W:
        return "Key_W";
    case Key_X:
        return "Key_X";
    case Key_Y:
        return "Key_Y";
    case Key_Z:
        return "Key_Z";
    case Key_Space:
        return "Key_Space";
    case Key_Enter:
        return "Key_Enter";
    case Key_CapsLock:
        return "Key_CapsLock";
    case Key_LeftCtrl:
        return "Key_LeftCtrl";
    case Key_LeftShift:
        return "Key_LeftShift";
    case Key_LeftAlt:
        return "Key_LeftAlt";
    case Key_RightCtrl:
        return "Key_RightCtrl";
    case Key_RightShift:
        return "Key_RightShift";
    case Key_RightAlt:
        return "Key_RightAlt";
    case Key_Up:
        return "Key_Up";
    case Key_Left:
        return "Key_Left";
    case Key_Down:
        return "Key_Down";
    case Key_Right:
        return "Key_Right";
    case Key_PageUp:
        return "Key_PageUp";
    case Key_PageDown:
        return "Key_PageDown";
    case Key_Home:
        return "Key_Home";
    case Key_End:
        return "Key_End";
    case Key_Pause:
        return "Key_Pause";
    case Key_ForwardSlash:
        return "Key_ForwardSlash";
    case Key_BackSlash:
        return "Key_BackSlash";
    case Key_Period:
        return "Key_Period";
    case Key_Comma:
        return "Key_Comma";
    case Key_Quote:
        return "Key_Quote";
    case Key_Semicolon:
        return "Key_Semicolon";
    case Key_LeftBracket:
        return "Key_LeftBracket";
    case Key_RightBracket:
        return "Key_RightBracket";
    case Keypad_Decimal:
        return "Keypad_Decimal";
    case Keypad_Multiply:
        return "Keypad_Multiply";
    case Keypad_Plus:
        return "Keypad_Plus";
    case Keypad_Clear:
        return "Keypad_Clear";
    case Keypad_Divide:
        return "Keypad_Divide";
    case Keypad_Enter:
        return "Keypad_Enter";
    case Keypad_Minus:
        return "Keypad_Minus";
    case Keypad_Equals:
        return "Keypad_Equals";
    case Keypad_0:
        return "Keypad_0";
    case Keypad_1:
        return "Keypad_1";
    case Keypad_2:
        return "Keypad_2";
    case Keypad_3:
        return "Keypad_3";
    case Keypad_4:
        return "Keypad_4";
    case Keypad_5:
        return "Keypad_5";
    case Keypad_6:
        return "Keypad_6";
    case Keypad_7:
        return "Keypad_7";
    case Keypad_8:
        return "Keypad_8";
    case Keypad_9:
        return "Keypad_9";
    case Key_Max:
        return "Key_Max";
    default:
        return KeyToString(Key_Unknown);
    }
}

void InputInit(void)
{
    memset(&g_Input, 0, sizeof(g_Input));
}

void InputUpdate(void)
{
    memcpy(g_Input.keysDownPrev, g_Input.keysDown, sizeof(g_Input.keysDown));
    memcpy(g_Input.mouseButtonsDownPrev, g_Input.mouseButtonsDown,
           sizeof(g_Input.mouseButtonsDown));

    g_Input.mousePosPrev.x = g_Input.mousePos.x;
    g_Input.mousePosPrev.y = g_Input.mousePos.y;
}

b32 KeyPressed(i32 key)
{
    return !g_Input.keysDownPrev[key] && g_Input.keysDown[key];
}

b32 KeyReleased(i32 key)
{
    return g_Input.keysDownPrev[key] && !g_Input.keysDown[key];
}

b32 KeyDown(i32 key)
{
    return g_Input.keysDown[key];
}

v2 MouseGetPos(void)
{
    return g_Input.mousePos;
}

void OnKeyPress(i32 key)
{
    g_Input.keysDown[key] = true;
}

void OnKeyRelease(i32 key)
{
    g_Input.keysDown[key] = false;
}

void OnKeyRepeat(i32 key)
{
    (void)key;
}

void OnMouseClick(i32 button)
{
    g_Input.mouseButtonsDown[button] = true;
}

void OnMouseRelease(i32 button)
{
    g_Input.mouseButtonsDown[button] = false;
}

void OnMouseMove(i32 x, i32 y)
{
    g_Input.mousePos = v2((f32)x, (f32)y);
}
