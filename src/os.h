enum os_key_t
{
    OS_Key_Null,
    OS_Key_Esc,
    OS_Key_F1,
    OS_Key_F2,
    OS_Key_F3,
    OS_Key_F4,
    OS_Key_F5,
    OS_Key_F6,
    OS_Key_F7,
    OS_Key_F8,
    OS_Key_F9,
    OS_Key_F10,
    OS_Key_F11,
    OS_Key_F12,
    OS_Key_GraveAccent,
    OS_Key_0,
    OS_Key_1,
    OS_Key_2,
    OS_Key_3,
    OS_Key_4,
    OS_Key_5,
    OS_Key_6,
    OS_Key_7,
    OS_Key_8,
    OS_Key_9,
    OS_Key_Minus,
    OS_Key_Equal,
    OS_Key_Backspace,
    OS_Key_Delete,
    OS_Key_Tab,
    OS_Key_A,
    OS_Key_B,
    OS_Key_C,
    OS_Key_D,
    OS_Key_E,
    OS_Key_F,
    OS_Key_G,
    OS_Key_H,
    OS_Key_I,
    OS_Key_J,
    OS_Key_K,
    OS_Key_L,
    OS_Key_M,
    OS_Key_N,
    OS_Key_O,
    OS_Key_P,
    OS_Key_Q,
    OS_Key_R,
    OS_Key_S,
    OS_Key_T,
    OS_Key_U,
    OS_Key_V,
    OS_Key_W,
    OS_Key_X,
    OS_Key_Y,
    OS_Key_Z,
    OS_Key_Space,
    OS_Key_Enter,
    OS_Key_Ctrl,
    OS_Key_Shift,
    OS_Key_Alt,
    OS_Key_Up,
    OS_Key_Left,
    OS_Key_Down,
    OS_Key_Right,
    OS_Key_PageUp,
    OS_Key_PageDown,
    OS_Key_Home,
    OS_Key_End,
    OS_Key_ForwardSlash,
    OS_Key_Period,
    OS_Key_Comma,
    OS_Key_Quote,
    OS_Key_LeftBracket,
    OS_Key_RightBracket,
    OS_Key_Max
};

enum os_gamepad_button_t
{
    OS_GamepadButton_Null,
    OS_GamepadButton_DPadUp,
    OS_GamepadButton_DPadDown,
    OS_GamepadButton_DPadLeft,
    OS_GamepadButton_DPadRight,
    OS_GamepadButton_Start,
    OS_GamepadButton_Back,
    OS_GamepadButton_LeftThumb,
    OS_GamepadButton_RightThumb,
    OS_GamepadButton_LeftShoulder,
    OS_GamepadButton_RightShoulder,
    OS_GamepadButton_A,
    OS_GamepadButton_B,
    OS_GamepadButton_X,
    OS_GamepadButton_Y,
    OS_GamepadButton_Max
};

enum os_event_type_t
{
    OS_EventType_Null,

    OS_EventType_WindowResize,

    OS_EventType_KeyStart,
    OS_EventType_CharacterInput,
    OS_EventType_KeyPress, // event fires once when key is down
    OS_EventType_KeyDown,  // event fires multiple times while key is held
    OS_EventType_KeyUp,
    OS_EventType_KeyEnd,

    OS_EventType_MouseStart,
    OS_EventType_MouseDown,
    OS_EventType_MouseUp,
    OS_EventType_MouseMove,
    OS_EventType_MouseScroll,
    OS_EventType_MouseEnd,

    OS_EventType_GamepadStart,
    OS_EventType_GamepadConnect,
    OS_EventType_GamepadDisconnect,
    OS_EventType_GamepadButtonPress,
    OS_EventType_GamepadButtonRelease,
    OS_EventType_GamepadJoystickMove,
    OS_EventType_GamepadTrigger,
    OS_EventType_GamepadEnd,

    OS_EventType_Max
};

enum os_mouse_button_t
{
    OS_MouseButton_Null,
    OS_MouseButton_Left,
    OS_MouseButton_Right,
    OS_MouseButton_Middle,
    OS_MouseButton_4,
    OS_MouseButton_5,
    OS_MouseButton_Max
};

enum os_key_modifiers_t
{
    OS_KeyModifier_Ctrl = (1 << 0),
    OS_KeyModifier_Shift = (1 << 1),
    OS_KeyModifier_Alt = (1 << 2)
};

struct os_event_t
{
    os_event_type_t type;
    os_mouse_button_t mouseButton;
    v2 delta;
    os_key_t key;
    u32 modifiers;
    // os_gamepad_button gamepadButton;
    // i32 gamepadIndex;
    // u64 character;
    // v2 position;
    // v2 scroll;
};

struct os_state_t
{
    volatile b32 running;
    u32 windowWidth;
    u32 windowHeight;
    b32 fullscreen;
    f32 deltaTime;

    i16 *sampleOut;
    u32 sampleCount;
    u32 samplesPerSecond;

    v2 mousePosition;
    u32 eventCount;
    os_event_t events[128];
};

// forward declare platform specific functions
internal void *OS_Reserve(u64 size);
internal void OS_Release(void *memory, u64 size);
internal void OS_Commit(void *memory, u64 size);
internal void OS_Decommit(void *memory, u64 size);
internal u32 OS_ReadFile(memory_arena_t *arena, char *path, void **data);
internal void OS_ShowCursor(b32 shown);
internal void OS_GLSwapBuffers(void);
internal void *OS_LoadOpenGLProcedure(char *name);
internal void OS_DebugPrint(char *format, ...);
internal void OS_DisplayError(char *format, ...);
