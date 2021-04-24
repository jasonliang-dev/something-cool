#include "x11_input.h"
#include <string.h>

static struct
{
    i32 keycodeTable[256];
} g_Input;

static i32 X11_TranslateKeysym(KeySym keysym)
{
    switch (keysym)
    {
    case XK_Alt_L:
        return Key_LeftAlt;
    case XK_Alt_R:
        return Key_RightAlt;
    case XK_BackSpace:
        return Key_Backspace;
    case XK_Caps_Lock:
        return Key_CapsLock;
    case XK_comma:
        return Key_Comma;
    case XK_Control_L:
        return Key_LeftCtrl;
    case XK_Control_R:
        return Key_RightCtrl;
    case XK_Delete:
        return Key_Delete;
    case XK_End:
        return Key_End;
    case XK_Escape:
        return Key_Esc;
    case XK_F1:
        return Key_F1;
    case XK_F2:
        return Key_F2;
    case XK_F3:
        return Key_F3;
    case XK_F4:
        return Key_F4;
    case XK_F5:
        return Key_F5;
    case XK_F6:
        return Key_F6;
    case XK_F7:
        return Key_F7;
    case XK_F8:
        return Key_F8;
    case XK_F9:
        return Key_F9;
    case XK_F10:
        return Key_F10;
    case XK_F11:
        return Key_F11;
    case XK_F12:
        return Key_F12;
    case XK_F13:
        return Key_F13;
    case XK_F14:
        return Key_F14;
    case XK_F15:
        return Key_F15;
    case XK_F16:
        return Key_F16;
    case XK_F17:
        return Key_F17;
    case XK_F18:
        return Key_F18;
    case XK_F19:
        return Key_F19;
    case XK_F20:
        return Key_F20;
    case XK_grave:
        return Key_Grave;
    case XK_Home:
        return Key_Home;
    case XK_Insert:
        return Key_Insert;
    case XK_KP_0:
        return Keypad_0;
    case XK_KP_1:
        return Keypad_1;
    case XK_KP_2:
        return Keypad_2;
    case XK_KP_3:
        return Keypad_3;
    case XK_KP_4:
        return Keypad_4;
    case XK_KP_5:
        return Keypad_5;
    case XK_KP_6:
        return Keypad_6;
    case XK_KP_7:
        return Keypad_7;
    case XK_KP_8:
        return Keypad_8;
    case XK_KP_9:
        return Keypad_9;
    case XK_KP_Add:
        return Keypad_Plus;
    case XK_KP_Decimal:
        return Keypad_Decimal;
    case XK_KP_Divide:
        return Keypad_Divide;
    case XK_KP_Enter:
        return Keypad_Enter;
    case XK_KP_Equal:
        return Keypad_Equals;
    case XK_KP_Multiply:
        return Keypad_Multiply;
    case XK_KP_Subtract:
        return Keypad_Minus;
    case XK_Next:
        return Key_PageDown;
    case XK_Pause:
        return Key_Pause;
    case XK_period:
        return Key_Period;
    case XK_slash:
        return Key_ForwardSlash;
    case XK_Prior:
        return Key_PageUp;
    case XK_Return:
        return Key_Enter;
    case XK_Shift_L:
        return Key_LeftShift;
    case XK_Shift_R:
        return Key_RightShift;
    case XK_Tab:
        return Key_Tab;
    case XK_space:
        return Key_Space;
    case XK_semicolon:
        return Key_Semicolon;
    case XK_apostrophe:
        return Key_Quote;
    case XK_backslash:
        return Key_BackSlash;
    case XK_bracketleft:
        return Key_LeftBracket;
    case XK_bracketright:
        return Key_RightBracket;
    case XK_minus:
        return Key_Minus;
    case XK_equal:
        return Key_Equal;

    case XK_Up:
        return Key_Up;
    case XK_Down:
        return Key_Down;
    case XK_Left:
        return Key_Left;
    case XK_Right:
        return Key_Right;

    case XK_0:
        return Key_0;
    case XK_1:
        return Key_1;
    case XK_2:
        return Key_2;
    case XK_3:
        return Key_3;
    case XK_4:
        return Key_4;
    case XK_5:
        return Key_5;
    case XK_6:
        return Key_6;
    case XK_7:
        return Key_7;
    case XK_8:
        return Key_8;
    case XK_9:
        return Key_9;

    case XK_a:
        return Key_A;
    case XK_b:
        return Key_B;
    case XK_c:
        return Key_C;
    case XK_d:
        return Key_D;
    case XK_e:
        return Key_E;
    case XK_f:
        return Key_F;
    case XK_g:
        return Key_G;
    case XK_h:
        return Key_H;
    case XK_i:
        return Key_I;
    case XK_j:
        return Key_J;
    case XK_k:
        return Key_K;
    case XK_l:
        return Key_L;
    case XK_m:
        return Key_M;
    case XK_n:
        return Key_N;
    case XK_o:
        return Key_O;
    case XK_p:
        return Key_P;
    case XK_q:
        return Key_Q;
    case XK_r:
        return Key_R;
    case XK_s:
        return Key_S;
    case XK_t:
        return Key_T;
    case XK_u:
        return Key_U;
    case XK_v:
        return Key_V;
    case XK_w:
        return Key_W;
    case XK_x:
        return Key_X;
    case XK_y:
        return Key_Y;
    case XK_z:
        return Key_Z;

    default:
        return Key_Unknown;
    }
}

void X11_InitInput(Display *display)
{
    memset(g_Input.keycodeTable, 0, sizeof(g_Input.keycodeTable));

    // x11 keycodes range from 8 to 255
    for (i32 i = 8; i <= 255; ++i)
    {
        int keysymsPerKeycode = 0;
        KeySym *keysym = XGetKeyboardMapping(display, i, 1, &keysymsPerKeycode);
        g_Input.keycodeTable[i] = X11_TranslateKeysym(*keysym);
        XFree(keysym);
    }
}

i32 X11_TranslateKeyEvent(XKeyEvent *event)
{
    if (event->keycode < 0 || event->keycode >= ArrayCount(g_Input.keycodeTable))
    {
        return Key_Unknown;
    }

    return g_Input.keycodeTable[event->keycode];
}

i32 X11_TranslateButtonEvent(XButtonEvent *event)
{
    switch (event->button)
    {
    case Button1:
        return Mouse_Left;
    case Button2:
        return Mouse_Middle;
    case Button3:
        return Mouse_Right;
    case Button4:
        return Mouse_4;
    case Button5:
        return Mouse_5;

    default:
        return Mouse_Unknown;
    }
}
