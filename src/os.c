typedef enum
{
#define Key(name, str) Key_##name
    Key(Null, "Invalid Key"),
    Key(Esc, "Escape"),
    Key(F1, "F1"),
    Key(F2, "F2"),
    Key(F3, "F3"),
    Key(F4, "F4"),
    Key(F5, "F5"),
    Key(F6, "F6"),
    Key(F7, "F7"),
    Key(F8, "F8"),
    Key(F9, "F9"),
    Key(F10, "F10"),
    Key(F11, "F11"),
    Key(F12, "F12"),
    Key(GraveAccent, "Grave Accent"),
    Key(0, "0"),
    Key(1, "1"),
    Key(2, "2"),
    Key(3, "3"),
    Key(4, "4"),
    Key(5, "5"),
    Key(6, "6"),
    Key(7, "7"),
    Key(8, "8"),
    Key(9, "9"),
    Key(Minus, "Minus"),
    Key(Equal, "Equal"),
    Key(Backspace, "Backspace"),
    Key(Delete, "Delete"),
    Key(Tab, "Tab"),
    Key(A, "A"),
    Key(B, "B"),
    Key(C, "C"),
    Key(D, "D"),
    Key(E, "E"),
    Key(F, "F"),
    Key(G, "G"),
    Key(H, "H"),
    Key(I, "I"),
    Key(J, "J"),
    Key(K, "K"),
    Key(L, "L"),
    Key(M, "M"),
    Key(N, "N"),
    Key(O, "O"),
    Key(P, "P"),
    Key(Q, "Q"),
    Key(R, "R"),
    Key(S, "S"),
    Key(T, "T"),
    Key(U, "U"),
    Key(V, "V"),
    Key(W, "W"),
    Key(X, "X"),
    Key(Y, "Y"),
    Key(Z, "Z"),
    Key(Space, "Space"),
    Key(Enter, "Enter"),
    Key(Ctrl, "Ctrl"),
    Key(Shift, "Shift"),
    Key(Alt, "Alt"),
    Key(Up, "Up"),
    Key(Left, "Left"),
    Key(Down, "Down"),
    Key(Right, "Right"),
    Key(PageUp, "Page Up"),
    Key(PageDown, "Page Down"),
    Key(Home, "Home"),
    Key(End, "End"),
    Key(ForwardSlash, "Forward Slash"),
    Key(Period, "Period"),
    Key(Comma, "Comma"),
    Key(Quote, "Quote"),
    Key(LeftBracket, "Left Bracket"),
    Key(RightBracket, "Right Bracket"),
#undef Key
    Key_Max
} os_keys;

typedef enum
{
#define GamepadButton(name, str) GamepadButton_##name
    GamepadButton(Null, "Invalid Gamepad Button"),
    GamepadButton(DPadUp, "D-Pad Up"),
    GamepadButton(DPadDown, "D-Pad Down"),
    GamepadButton(DPadLeft, "D-Pad Left"),
    GamepadButton(DPadRight, "D-Pad Right"),
    GamepadButton(Start, "Start"),
    GamepadButton(Back, "Back"),
    GamepadButton(LeftThumb, "Left Thumb"),
    GamepadButton(RightThumb, "Right Thumb"),
    GamepadButton(LeftShoulder, "Left Shoulder"),
    GamepadButton(RightShoulder, "Right Shoulder"),
    GamepadButton(A, "A"),
    GamepadButton(B, "B"),
    GamepadButton(X, "X"),
    GamepadButton(Y, "Y"),
#undef GamepadButton
    GamepadButton_Max
} os_gamepad_button;

typedef struct
{
    volatile b32 quit;
    void (*DebugPrint)(char *str);
} os_state;

global os_state *os = 0;

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
