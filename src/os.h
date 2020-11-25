typedef enum os_event_type os_event_type;
enum os_event_type
{
    OS_EventType_Null,

    OS_EventType_KeyStart,
    OS_EventType_CharacterInput,
    OS_EventType_KeyPress,
    OS_EventType_KeyRelease,
    OS_EventType_KeyEnd,

    OS_EventType_MouseStart,
    OS_EventType_MousePress,
    OS_EventType_MouseRelease,
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

typedef struct os_event os_event;
struct os_event
{
    os_event_type type;
    // os_key key;
    // os_gamepad_button gamepadButton;
    u32 mouseButton;
    // u32 modifiers;
    // i32 gamepadIndex;
    // u64 character;
    // v2 position;
    v2 delta;
    // v2 scroll;
};

typedef struct os_state os_state;
struct os_state
{
    volatile b32 running;
    iv2 windowResolution;

    f32 *sampleOut;
    u32 sampleCount;
    u32 samplesPerSecond;

    v2 mousePosition;
    i32 eventCount;
    os_event events[4096];

    void *(*Reserve)(u64 size);
    void (*Release)(void *memory);
    void (*Commit)(void *memory, u64 size);
    void (*Decommit)(void *memory, u64 size);
    void (*DebugPrint)(char *str);
    void (*GLSwapBuffers)(void);

    memory_arena permanentArena;
    memory_arena frameArena;
};

global os_state *os = 0;

typedef enum os_mouse_button os_mouse_button;
enum os_mouse_button
{
    MouseButton_Null,
    MouseButton_Left,
    MouseButton_Right,
    MouseButton_Middle,
    MouseButton_4,
    MouseButton_5,
    MouseButton_Max
};

typedef enum os_key_modifiers os_key_modifiers;
enum os_key_modifiers
{
    KeyModifier_Ctrl = (1 << 0),
    KeyModifier_Shift = (1 << 1),
    KeyModifier_Alt = (1 << 2)
};

typedef enum os_key os_key;
enum os_key
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

typedef enum os_gamepad_button os_gamepad_button;
enum os_gamepad_button
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
