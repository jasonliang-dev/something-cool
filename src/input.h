#pragma once

#include "language.h"
#include "geometry.h"

enum
{
    Key_Unknown,
    Key_Esc,
    Key_F1,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12,
    Key_F13,
    Key_F14,
    Key_F15,
    Key_F16,
    Key_F17,
    Key_F18,
    Key_F19,
    Key_F20,
    Key_Grave,
    Key_0,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,
    Key_Minus,
    Key_Equal,
    Key_Backspace,
    Key_Insert,
    Key_Delete,
    Key_Tab,
    Key_A,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z,
    Key_Space,
    Key_Enter,
    Key_CapsLock,
    Key_LeftCtrl,
    Key_LeftShift,
    Key_LeftAlt,
    Key_RightCtrl,
    Key_RightShift,
    Key_RightAlt,
    Key_Up,
    Key_Left,
    Key_Down,
    Key_Right,
    Key_PageUp,
    Key_PageDown,
    Key_Home,
    Key_End,
    Key_Pause,
    Key_ForwardSlash,
    Key_BackSlash,
    Key_Period,
    Key_Comma,
    Key_Quote,
    Key_Semicolon,
    Key_LeftBracket,
    Key_RightBracket,
    Keypad_Decimal,
    Keypad_Multiply,
    Keypad_Plus,
    Keypad_Clear,
    Keypad_Divide,
    Keypad_Enter,
    Keypad_Minus,
    Keypad_Equals,
    Keypad_0,
    Keypad_1,
    Keypad_2,
    Keypad_3,
    Keypad_4,
    Keypad_5,
    Keypad_6,
    Keypad_7,
    Keypad_8,
    Keypad_9,
    Key_Max,
};

enum
{
    Mouse_Unknown,
    Mouse_Left,
    Mouse_Right,
    Mouse_Middle,
    Mouse_4,
    Mouse_5,
    Mouse_Max,
};

const char *KeyToString(i32 key);

void InputInit(void);
void InputUpdate(void);

b32 KeyPressed(i32 key);
b32 KeyReleased(i32 key);
b32 KeyDown(i32 key);

v2 GetMousePos(void);

// only window events should call these functions below
void OnKeyPress(i32 key);
void OnKeyRelease(i32 key);
void OnKeyRepeat(i32 key);

void OnMouseClick(i32 button);
void OnMouseRelease(i32 button);

void OnMouseMove(i32 x, i32 y);
