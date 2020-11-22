#include <xinput.h>

#define W32_MAX_GAMEPADS 4

typedef struct w32_gamepad_input w32_gamepad_input;
struct w32_gamepad_input
{
    b32 connected;
    v2 joystick1;
    v2 joystick2;
    f32 triggerLeft;
    f32 triggerRight;
    i32 buttonStates[GamepadButton_Max];
};

w32_gamepad_input globalGamepads[W32_MAX_GAMEPADS];

#define X_INPUT_GET_STATE(name) DWORD name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state_fn);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    (void)dwUserIndex;
    (void)pState;
    return ERROR_DEVICE_NOT_CONNECTED;
}
global x_input_get_state_fn *XInputGetStateProc = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) DWORD name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state_fn);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    (void)dwUserIndex;
    (void)pVibration;
    return ERROR_DEVICE_NOT_CONNECTED;
}
global x_input_set_state_fn *XInputSetStateProc = XInputSetStateStub;

internal void W32_LoadXInput(void)
{
    HMODULE xinputLib = LoadLibraryA("xinput1_4.dll");

    if (xinputLib == NULL)
    {
        xinputLib = LoadLibraryA("xinput9_1_0.dll");
    }

    if (xinputLib == NULL)
    {
        xinputLib = LoadLibraryA("xinput1_3.dll");
    }

    if (xinputLib)
    {
        XInputGetStateProc = (x_input_get_state_fn *)GetProcAddress(xinputLib, "XInputGetState");
        XInputSetStateProc = (x_input_set_state_fn *)GetProcAddress(xinputLib, "XInputSetState");
    }
}

internal void W32_UpdateXInput(void)
{
    u32 controllerCount = Min(W32_MAX_GAMEPADS, XUSER_MAX_COUNT);

    for (u32 i = 0; i < controllerCount; ++i)
    {
        XINPUT_STATE controller_state = {0};
        if (XInputGetStateProc(0, &controller_state) == ERROR_SUCCESS)
        {
            globalGamepads[i].connected = 1;
            XINPUT_GAMEPAD *pad = &controller_state.Gamepad;

            for (u32 j = 0; j < GamepadButton_Max; ++j)
            {
                globalGamepads[i].buttonStates[j] = 0;
            }

            globalGamepads[i].buttonStates[GamepadButton_DPadUp] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
            globalGamepads[i].buttonStates[GamepadButton_DPadDown] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
            globalGamepads[i].buttonStates[GamepadButton_DPadLeft] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
            globalGamepads[i].buttonStates[GamepadButton_DPadRight] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
            globalGamepads[i].buttonStates[GamepadButton_Start] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_START);
            globalGamepads[i].buttonStates[GamepadButton_Back] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_BACK);
            globalGamepads[i].buttonStates[GamepadButton_LeftThumb] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
            globalGamepads[i].buttonStates[GamepadButton_RightThumb] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
            globalGamepads[i].buttonStates[GamepadButton_LeftShoulder] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            globalGamepads[i].buttonStates[GamepadButton_RightShoulder] |=
                !!(pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            globalGamepads[i].buttonStates[GamepadButton_A] |= !!(pad->wButtons & XINPUT_GAMEPAD_A);
            globalGamepads[i].buttonStates[GamepadButton_B] |= !!(pad->wButtons & XINPUT_GAMEPAD_B);
            globalGamepads[i].buttonStates[GamepadButton_X] |= !!(pad->wButtons & XINPUT_GAMEPAD_X);
            globalGamepads[i].buttonStates[GamepadButton_Y] |= !!(pad->wButtons & XINPUT_GAMEPAD_Y);

            globalGamepads[i].joystick1.x = pad->sThumbLX / 32768.f;
            globalGamepads[i].joystick1.y = pad->sThumbLY / 32768.f;
            globalGamepads[i].joystick2.x = pad->sThumbRX / 32768.f;
            globalGamepads[i].joystick2.y = pad->sThumbRY / 32768.f;
        }
        else
        {
            globalGamepads[i].connected = 0;
        }
    }
}
