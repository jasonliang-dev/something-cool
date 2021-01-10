#include <windows.h>
#include <strsafe.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <xinput.h>

#include "app.cpp"

global os_state_t globalOS;
global HDC globalHDC;

#include "win32_file_io.cpp"
#include "win32_opengl.cpp"
#include "win32_os.cpp"
#include "win32_timer.cpp"
#include "win32_utils.cpp"
#include "win32_wasapi.cpp"
#include "win32_xinput.cpp"

internal LRESULT CALLBACK W32_WindowProcedure(HWND window, UINT message, WPARAM wParam,
                                              LPARAM lParam)
{
    u32 modifiers = 0;
    os_event_t event;

    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
        modifiers |= KeyModifier_Ctrl;
    }
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        modifiers |= KeyModifier_Shift;
    }
    if (GetKeyState(VK_MENU) & 0x8000)
    {
        modifiers |= KeyModifier_Alt;
    }

    switch (message)
    {
    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
        globalOS.running = false;
        return 0;
    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);
    case WM_SIZE: {
        i32 width = LOWORD(lParam);
        i32 height = HIWORD(lParam);
        event.type = OS_EventType_WindowResize;
        event.delta =
            v2((f32)(width - globalOS.windowWidth), (f32)(height - globalOS.windowHeight));
        globalOS.windowWidth = width;
        globalOS.windowHeight = height;
        OS_PushEvent(event);
        return 0;
    }
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
        b8 isDown = !(lParam & (1 << 31));
        b8 prevDown = !!(lParam & (1 << 30));

        u64 keyInput = 0;
        W32_KeyCodeToOSKey(wParam, &keyInput, &modifiers);

        if (!isDown)
        {
            event.type = OS_EventType_KeyUp;
        }
        else
        {
            if (!prevDown)
            {
                os_event_t eventOnce;
                eventOnce.type = OS_EventType_KeyDown;
                eventOnce.key = (os_key_t)keyInput;
                eventOnce.modifiers = modifiers;
                OS_PushEvent(eventOnce);
            }

            event.type = OS_EventType_KeyPress;
        }

        event.key = (os_key_t)keyInput;
        event.modifiers = modifiers;
        OS_PushEvent(event);

        return DefWindowProc(window, message, wParam, lParam);
    }
    case WM_LBUTTONDOWN:
        event.type = OS_EventType_MouseDown;
        event.mouseButton = MouseButton_Left;
        OS_PushEvent(event);
        return 0;
    case WM_LBUTTONUP:
        event.type = OS_EventType_MouseUp;
        event.mouseButton = MouseButton_Left;
        OS_PushEvent(event);
        return 0;
    case WM_RBUTTONDOWN:
        event.type = OS_EventType_MouseDown;
        event.mouseButton = MouseButton_Right;
        OS_PushEvent(event);
        return 0;
    case WM_RBUTTONUP:
        event.type = OS_EventType_MouseUp;
        event.mouseButton = MouseButton_Right;
        OS_PushEvent(event);
        return 0;
    case WM_MOUSEMOVE:
        v2 lastMouse = globalOS.mousePosition;
        event.type = OS_EventType_MouseMove;
        event.delta = V2MinusV2(globalOS.mousePosition, lastMouse);
        globalOS.mousePosition = W32_GetMousePosition(window);
        OS_PushEvent(event);
        return 0;
    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine,
                     int commandShow)
{
    (void)previousInstance;
    (void)commandLine;

    WNDCLASS windowClass = {0};
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = W32_WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "ApplicationWindowClass";
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&windowClass))
    {
        OS_DisplayError("Cannot register window class");
        return 1;
    }

    HWND window = CreateWindow("ApplicationWindowClass", WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH,
                               DEFAULT_WINDOW_HEIGHT, NULL, NULL, instance, NULL);

    if (!window)
    {
        OS_DisplayError("Cannot create window");
        return 1;
    }

    w32_sound_output_t soundOutput = {0};
    soundOutput.channels = 2;
    soundOutput.samplesPerSecond = AUDIO_SAMPLE_RATE;
    W32_LoadWASAPI();

    if (!W32_InitWASAPI(&soundOutput))
    {
        return 1;
    }

    os = &globalOS;
    globalOS.running = true;
    globalOS.windowWidth = DEFAULT_WINDOW_WIDTH;
    globalOS.windowHeight = DEFAULT_WINDOW_HEIGHT;
    globalOS.fullscreen = false;

    globalOS.sampleOut = (i16 *)VirtualAlloc(0, soundOutput.samplesPerSecond * sizeof(i16) * 2,
                                             MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    globalOS.samplesPerSecond = soundOutput.samplesPerSecond;

    globalHDC = GetDC(window);
    HGLRC glContext = W32_InitOpenGL(globalHDC);

    W32_LoadXInput();

    AppLoad(&globalOS);

    ShowWindow(window, commandShow);
    UpdateWindow(window);

    w32_timer_t timer;
    W32_InitTimer(&timer);

    MSG message;
    while (globalOS.running)
    {
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                globalOS.running = false;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        RECT clientRect;
        GetClientRect(window, &clientRect);
        globalOS.windowWidth = clientRect.right - clientRect.left;
        globalOS.windowHeight = clientRect.bottom - clientRect.top;

        W32_UpdateXInput();

        globalOS.sampleCount = 0;
        u32 soundPaddingSize;
        if (SUCCEEDED(soundOutput.audioClient->GetCurrentPadding(&soundPaddingSize)))
        {
            globalOS.samplesPerSecond = soundOutput.samplesPerSecond;
            globalOS.sampleCount = (u32)(soundOutput.latencyFrameCount - soundPaddingSize);
            if (globalOS.sampleCount > soundOutput.latencyFrameCount)
            {
                globalOS.sampleCount = soundOutput.latencyFrameCount;
            }
        }

        MemorySet(globalOS.sampleOut, 0, soundOutput.bufferFrameCount);

        b32 last_fullscreen = globalOS.fullscreen;
        AppUpdate();

        if (last_fullscreen != globalOS.fullscreen)
        {
            W32_ToggleFullscreen(window);
        }

        W32_FillSoundBuffer(globalOS.sampleCount, globalOS.sampleOut, &soundOutput);

        W32_TimerEndFrame(&timer);
    }

    AppClose();
    ShowWindow(window, SW_HIDE);
    W32_CleanUpOpenGL(&globalHDC, glContext);

    return 0;
}
