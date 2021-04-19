#pragma once

#include "language.h"

#define INPUT_KEY_LIST                                                                             \
    X(Key_Unknown, "Unknown")                                                                      \
    X(Key_Esc, "Esc")                                                                              \
    X(Key_F1, "F1")                                                                                \
    X(Key_F2, "F2")                                                                                \
    X(Key_F3, "F3")                                                                                \
    X(Key_F4, "F4")                                                                                \
    X(Key_F5, "F5")                                                                                \
    X(Key_F6, "F6")                                                                                \
    X(Key_F7, "F7")                                                                                \
    X(Key_F8, "F8")                                                                                \
    X(Key_F9, "F9")                                                                                \
    X(Key_F10, "F10")                                                                              \
    X(Key_F11, "F11")                                                                              \
    X(Key_F12, "F12")                                                                              \
    X(Key_F13, "F13")                                                                              \
    X(Key_F14, "F14")                                                                              \
    X(Key_F15, "F15")                                                                              \
    X(Key_F16, "F16")                                                                              \
    X(Key_F17, "F17")                                                                              \
    X(Key_F18, "F18")                                                                              \
    X(Key_F19, "F19")                                                                              \
    X(Key_F20, "F20")                                                                              \
    X(Key_Grave, "Grave")                                                                          \
    X(Key_0, "0")                                                                                  \
    X(Key_1, "1")                                                                                  \
    X(Key_2, "2")                                                                                  \
    X(Key_3, "3")                                                                                  \
    X(Key_4, "4")                                                                                  \
    X(Key_5, "5")                                                                                  \
    X(Key_6, "6")                                                                                  \
    X(Key_7, "7")                                                                                  \
    X(Key_8, "8")                                                                                  \
    X(Key_9, "9")                                                                                  \
    X(Key_Minus, "Minus")                                                                          \
    X(Key_Equal, "Equal")                                                                          \
    X(Key_Backspace, "Backspace")                                                                  \
    X(Key_Delete, "Delete")                                                                        \
    X(Key_Tab, "Tab")                                                                              \
    X(Key_A, "A")                                                                                  \
    X(Key_B, "B")                                                                                  \
    X(Key_C, "C")                                                                                  \
    X(Key_D, "D")                                                                                  \
    X(Key_E, "E")                                                                                  \
    X(Key_F, "F")                                                                                  \
    X(Key_G, "G")                                                                                  \
    X(Key_H, "H")                                                                                  \
    X(Key_I, "I")                                                                                  \
    X(Key_J, "J")                                                                                  \
    X(Key_K, "K")                                                                                  \
    X(Key_L, "L")                                                                                  \
    X(Key_M, "M")                                                                                  \
    X(Key_N, "N")                                                                                  \
    X(Key_O, "O")                                                                                  \
    X(Key_P, "P")                                                                                  \
    X(Key_Q, "Q")                                                                                  \
    X(Key_R, "R")                                                                                  \
    X(Key_S, "S")                                                                                  \
    X(Key_T, "T")                                                                                  \
    X(Key_U, "U")                                                                                  \
    X(Key_V, "V")                                                                                  \
    X(Key_W, "W")                                                                                  \
    X(Key_X, "X")                                                                                  \
    X(Key_Y, "Y")                                                                                  \
    X(Key_Z, "Z")                                                                                  \
    X(Key_Space, "Space")                                                                          \
    X(Key_Enter, "Enter")                                                                          \
    X(Key_LeftCtrl, "LeftCtrl")                                                                    \
    X(Key_LeftShift, "LeftShift")                                                                  \
    X(Key_LeftAlt, "LeftAlt")                                                                      \
    X(Key_RightCtrl, "RightCtrl")                                                                  \
    X(Key_RightShift, "RightShift")                                                                \
    X(Key_RightAlt, "RightAlt")                                                                    \
    X(Key_Up, "Up")                                                                                \
    X(Key_Left, "Left")                                                                            \
    X(Key_Down, "Down")                                                                            \
    X(Key_Right, "Right")                                                                          \
    X(Key_PageUp, "PageUp")                                                                        \
    X(Key_PageDown, "PageDown")                                                                    \
    X(Key_Home, "Home")                                                                            \
    X(Key_End, "End")                                                                              \
    X(Key_ForwardSlash, "ForwardSlash")                                                            \
    X(Key_BackSlash, "BackSlash")                                                                  \
    X(Key_Period, "Period")                                                                        \
    X(Key_Comma, "Comma")                                                                          \
    X(Key_Quote, "Quote")                                                                          \
    X(Key_Semicolon, "Semicolon")                                                                  \
    X(Key_LeftBracket, "LeftBracket")                                                              \
    X(Key_RightBracket, "RightBracket")                                                            \
    X(Keypad_Decimal, "Decimal")                                                                   \
    X(Keypad_Multiply, "Multiply")                                                                 \
    X(Keypad_Plus, "Plus")                                                                         \
    X(Keypad_Clear, "Clear")                                                                       \
    X(Keypad_Divide, "Divide")                                                                     \
    X(Keypad_Enter, "Enter")                                                                       \
    X(Keypad_Minus, "Minus")                                                                       \
    X(Keypad_Equals, "Equals")                                                                     \
    X(Keypad_0, "0")                                                                               \
    X(Keypad_1, "1")                                                                               \
    X(Keypad_2, "2")                                                                               \
    X(Keypad_3, "3")                                                                               \
    X(Keypad_4, "4")                                                                               \
    X(Keypad_5, "5")                                                                               \
    X(Keypad_6, "6")                                                                               \
    X(Keypad_7, "7")                                                                               \
    X(Keypad_8, "8")                                                                               \
    X(Keypad_9, "9")                                                                               \
    X(Key_Max, "Max")

enum
{
#define X(name, string) name,
    INPUT_KEY_LIST
#undef X
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
void OnKeyPress(i32 key);
void OnKeyRelease(i32 key);
void OnKeyRepeat(i32 key);

void OnMouseClick(i32 button);
void OnMouseRelease(i32 button);

void OnMouseMove(i32 x, i32 y);
