typedef enum os_key_t os_key_t;
enum os_key_t
{
    Key_Null,
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
    Key_GraveAccent,
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
    Key_Ctrl,
    Key_Shift,
    Key_Alt,
    Key_Up,
    Key_Left,
    Key_Down,
    Key_Right,
    Key_PageUp,
    Key_PageDown,
    Key_Home,
    Key_End,
    Key_ForwardSlash,
    Key_Period,
    Key_Comma,
    Key_Quote,
    Key_LeftBracket,
    Key_RightBracket,
    Key_Max
};

typedef enum os_gamepad_button_t os_gamepad_button_t;
enum os_gamepad_button_t
{
    GamepadButton_Null,
    GamepadButton_DPadUp,
    GamepadButton_DPadDown,
    GamepadButton_DPadLeft,
    GamepadButton_DPadRight,
    GamepadButton_Start,
    GamepadButton_Back,
    GamepadButton_LeftThumb,
    GamepadButton_RightThumb,
    GamepadButton_LeftShoulder,
    GamepadButton_RightShoulder,
    GamepadButton_A,
    GamepadButton_B,
    GamepadButton_X,
    GamepadButton_Y,
    GamepadButton_Max
};

typedef enum os_event_type_t os_event_type_t;
enum os_event_type_t
{
    OS_EventType_Null,

    OS_EventType_WindowResize,

    OS_EventType_KeyStart,
    OS_EventType_CharacterInput,
    OS_EventType_KeyPress, // can fire multiple times while key is held
    OS_EventType_KeyDown,  // should fires once when key is pressed down
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

typedef enum os_mouse_button_t os_mouse_button_t;
enum os_mouse_button_t
{
    MouseButton_Null,
    MouseButton_Left,
    MouseButton_Right,
    MouseButton_Middle,
    MouseButton_4,
    MouseButton_5,
    MouseButton_Max
};

typedef enum os_key_modifiers_t os_key_modifiers_t;
enum os_key_modifiers_t
{
    KeyModifier_Ctrl = (1 << 0),
    KeyModifier_Shift = (1 << 1),
    KeyModifier_Alt = (1 << 2)
};

typedef struct os_event_t os_event_t;
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

typedef struct os_state_t os_state_t;
struct os_state_t
{
    volatile b32 running;
    iv2 windowResolution;
    b32 fullscreen;

    f32 *sampleOut;
    u32 sampleCount;
    u32 samplesPerSecond;

    v2 mousePosition;
    i32 eventCount;
    os_event_t events[4096];

    memory_arena_t permanentArena;
};

internal void *OS_Reserve(u64 size);
internal void OS_Release(void *memory, u64 size);
internal void OS_Commit(void *memory, u64 size);
internal void OS_Decommit(void *memory, u64 size);
internal void OS_ReadFile(memory_arena_t *arena, char *path, void **data, u64 *len);
internal void OS_ShowCursor(b32 shown);
internal void OS_GLSwapBuffers(void);
internal void *OS_LoadOpenGLProcedure(char *name);
internal void OS_DebugPrint(char *format, ...);
internal void OS_DisplayError(char *format, ...);
