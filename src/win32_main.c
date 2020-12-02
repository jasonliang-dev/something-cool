#include <windows.h>
#include <strsafe.h>

#include "app.c"

global os_state globalOS;
global HDC globalHDC;

#include "win32_utils.c"
#include "win32_file_io.c"
#include "win32_opengl.c"
#include "win32_timer.c"
#include "win32_wasapi.c"
#include "win32_xinput.c"

internal LRESULT CALLBACK W32_WindowProcedure(HWND window, UINT message, WPARAM wParam,
                                              LPARAM lParam)
{
    u32 modifiers = 0;
    os_event event;

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

    if (message == WM_CLOSE || message == WM_DESTROY || message == WM_QUIT)
    {
        globalOS.running = 0;
        return 0;
    }
    else if (message == WM_MENUCHAR)
    {
        return MAKELRESULT(0, MNC_CLOSE);
    }
    else if (message == WM_SIZE)
    {
        i32 width = LOWORD(lParam);
        i32 height = HIWORD(lParam);
        event.type = OS_EventType_WindowResize;
        event.delta = v2((f32)(width - globalOS.windowResolution.x),
                         (f32)(height - globalOS.windowResolution.y));
        globalOS.windowResolution.x = width;
        globalOS.windowResolution.y = height;
        OS_PushEvent(event);
        return 0;
    }
    else if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP || message == WM_KEYDOWN ||
             message == WM_KEYUP)
    {
        u64 vkeyCode = wParam;
        u64 keyInput = 0;

        if ((vkeyCode >= 'A' && vkeyCode <= 'Z') || (vkeyCode >= '0' && vkeyCode <= '9'))
        {
            keyInput = (vkeyCode >= 'A' && vkeyCode <= 'Z') ? Key_A + (vkeyCode - 'A')
                                                            : Key_0 + (vkeyCode - '0');
        }
        else if (vkeyCode >= VK_F1 && vkeyCode <= VK_F12)
        {
            keyInput = Key_F1 + vkeyCode - VK_F1;
        }
        else if (vkeyCode == VK_ESCAPE)
        {
            keyInput = Key_Esc;
        }
        else if (vkeyCode == VK_OEM_3)
        {
            keyInput = Key_GraveAccent;
        }
        else if (vkeyCode == VK_OEM_MINUS)
        {
            keyInput = Key_Minus;
        }
        else if (vkeyCode == VK_OEM_PLUS)
        {
            keyInput = Key_Equal;
        }
        else if (vkeyCode == VK_BACK)
        {
            keyInput = Key_Backspace;
        }
        else if (vkeyCode == VK_TAB)
        {
            keyInput = Key_Tab;
        }
        else if (vkeyCode == VK_SPACE)
        {
            keyInput = Key_Space;
        }
        else if (vkeyCode == VK_RETURN)
        {
            keyInput = Key_Enter;
        }
        else if (vkeyCode == VK_CONTROL)
        {
            keyInput = Key_Ctrl;
            modifiers &= ~KeyModifier_Ctrl;
        }
        else if (vkeyCode == VK_SHIFT)
        {
            keyInput = Key_Shift;
            modifiers &= ~KeyModifier_Shift;
        }
        else if (vkeyCode == VK_MENU)
        {
            keyInput = Key_Alt;
            modifiers &= ~KeyModifier_Alt;
        }
        else if (vkeyCode == VK_UP)
        {
            keyInput = Key_Up;
        }
        else if (vkeyCode == VK_LEFT)
        {
            keyInput = Key_Left;
        }
        else if (vkeyCode == VK_DOWN)
        {
            keyInput = Key_Down;
        }
        else if (vkeyCode == VK_RIGHT)
        {
            keyInput = Key_Right;
        }
        else if (vkeyCode == VK_DELETE)
        {
            keyInput = Key_Delete;
        }
        else if (vkeyCode == VK_PRIOR)
        {
            keyInput = Key_PageUp;
        }
        else if (vkeyCode == VK_NEXT)
        {
            keyInput = Key_PageDown;
        }
        else if (vkeyCode == VK_HOME)
        {
            keyInput = Key_Home;
        }
        else if (vkeyCode == VK_END)
        {
            keyInput = Key_End;
        }
        else if (vkeyCode == VK_OEM_2)
        {
            keyInput = Key_ForwardSlash;
        }
        else if (vkeyCode == VK_OEM_PERIOD)
        {
            keyInput = Key_Period;
        }
        else if (vkeyCode == VK_OEM_COMMA)
        {
            keyInput = Key_Comma;
        }
        else if (vkeyCode == VK_OEM_7)
        {
            keyInput = Key_Quote;
        }
        else if (vkeyCode == VK_OEM_4)
        {
            keyInput = Key_LeftBracket;
        }
        else if (vkeyCode == VK_OEM_6)
        {
            keyInput = Key_RightBracket;
        }

        i8 isDown = !(lParam & (1 << 31));
        event.type = isDown ? OS_EventType_KeyPress : OS_EventType_KeyRelease;
        event.key = keyInput;
        event.modifiers = modifiers;
        OS_PushEvent(event);

        return DefWindowProc(window, message, wParam, lParam);
    }
    else if (message == WM_LBUTTONDOWN)
    {
        event.type = OS_EventType_MousePress;
        event.mouseButton = MouseButton_Left;
        OS_PushEvent(event);
        return 0;
    }
    else if (message == WM_LBUTTONUP)
    {
        event.type = OS_EventType_MouseRelease;
        event.mouseButton = MouseButton_Left;
        OS_PushEvent(event);
        return 0;
    }
    else if (message == WM_RBUTTONDOWN)
    {
        event.type = OS_EventType_MousePress;
        event.mouseButton = MouseButton_Right;
        OS_PushEvent(event);
        return 0;
    }
    else if (message == WM_RBUTTONUP)
    {
        event.type = OS_EventType_MouseRelease;
        event.mouseButton = MouseButton_Right;
        OS_PushEvent(event);
        return 0;
    }
    else if (message == WM_MOUSEMOVE)
    {
        v2 lastMouse = globalOS.mousePosition;
        event.type = OS_EventType_MouseMove;
        event.delta = V2MinusV2(globalOS.mousePosition, lastMouse);
        globalOS.mousePosition = W32_GetMousePosition(window);
        OS_PushEvent(event);
        return 0;
    }

    return DefWindowProc(window, message, wParam, lParam);
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

    w32_sound_output soundOutput = {0};
    soundOutput.channels = 2;
    soundOutput.samplesPerSecond = 48000;
    soundOutput.latencyFrameCount = 48000;
    W32_LoadWASAPI();
    W32_InitWASAPI(&soundOutput);

    os = &globalOS;
    globalOS.running = 1;
    globalOS.windowResolution.x = DEFAULT_WINDOW_WIDTH;
    globalOS.windowResolution.y = DEFAULT_WINDOW_HEIGHT;
    globalOS.fullscreen = 0;

    globalOS.sampleOut = VirtualAlloc(0, soundOutput.samplesPerSecond * sizeof(f32) * 2,
                                      MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    globalOS.samplesPerSecond = soundOutput.samplesPerSecond;

    globalOS.permanentArena = M_ArenaInitialize(Gigabytes(4));
    globalOS.frameArena = M_ArenaInitialize(Gigabytes(4));

    globalHDC = GetDC(window);
    HGLRC glContext = W32_InitOpenGL(globalHDC);

    W32_LoadXInput();

    AppLoad(&globalOS);

    ShowWindow(window, commandShow);
    UpdateWindow(window);

    w32_timer timer;
    W32_InitTimer(&timer);

    MSG message;
    while (globalOS.running)
    {
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                globalOS.running = 0;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        RECT clientRect;
        GetClientRect(window, &clientRect);
        globalOS.windowResolution.x = clientRect.right - clientRect.left;
        globalOS.windowResolution.y = clientRect.bottom - clientRect.top;

        W32_UpdateXInput();

        if (soundOutput.initialized)
        {
            globalOS.sampleCount = 0;
            UINT32 soundPaddingSize;
            if (SUCCEEDED(soundOutput.audioClient->lpVtbl->GetCurrentPadding(
                    soundOutput.audioClient, &soundPaddingSize)))
            {
                globalOS.samplesPerSecond = soundOutput.samplesPerSecond;
                globalOS.sampleCount = (u32)(soundOutput.latencyFrameCount - soundPaddingSize);
                if (globalOS.sampleCount > soundOutput.latencyFrameCount)
                {
                    globalOS.sampleCount = soundOutput.latencyFrameCount;
                }
            }

            for (u32 i = 0; i < soundOutput.bufferFrameCount; i++)
            {
                globalOS.sampleOut[i] = 0;
            }
        }

        b32 last_fullscreen = globalOS.fullscreen;
        AppUpdate();

        if (last_fullscreen != globalOS.fullscreen)
        {
            W32_ToggleFullscreen(window);
        }

        if (soundOutput.initialized)
        {
            W32_FillSoundBuffer(globalOS.sampleCount, globalOS.sampleOut, &soundOutput);
        }

        W32_TimerEndFrame(&timer);
    }

    AppClose();
    ShowWindow(window, SW_HIDE);
    W32_CleanUpOpenGL(&globalHDC, glContext);

    return 0;
}
