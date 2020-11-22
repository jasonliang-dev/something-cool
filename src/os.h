typedef struct os_state os_state;
struct os_state
{
    volatile b32 running;

    i16 *sampleOut;
    u32 sampleCount;
    u32 samplesPerSecond;

    void *(*Reserve)(u64 size);
    void (*Release)(void *memory);
    void (*Commit)(void *memory, u64 size);
    void (*Decommit)(void *memory, u64 size);
    void (*DebugPrint)(char *str);

    memory_arena permanentArena;
    memory_arena frameArena;
};

global os_state *os = 0;

typedef enum os_keys os_keys;
enum os_keys
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

#ifdef _MSC_VER
#define APP_EXPORT __declspec(dllexport)
#else
#define APP_EXPORT
#endif

#define APP_PERMANENT_LOAD APP_EXPORT void PermanentLoad(os_state *os_)
typedef void app_permanent_load_fn(os_state *);
internal void AppPermanentLoadStub(os_state *os_)
{
    (void)os_;
}

#define APP_HOT_LOAD APP_EXPORT void HotLoad(os_state *os_)
typedef void app_hot_load_fn(os_state *);
internal void AppHotLoadStub(os_state *os_)
{
    (void)os_;
}

#define APP_HOT_UNLOAD APP_EXPORT void HotUnload(void)
typedef void app_hot_unload_fn(void);
internal void AppHotUnloadStub(void)
{
}

#define APP_UPDATE APP_EXPORT void Update(void)
typedef void app_update_fn(void);
internal void AppUpdateStub(void)
{
}
