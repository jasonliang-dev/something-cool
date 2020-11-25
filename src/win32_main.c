#include <windows.h>
#include <strsafe.h>

#include "app.h"

global os_state globalOS;
global HDC globalHDC;

#include "win32_utils.c"

#include "win32_app_code.c"
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
        event.delta =
            v2(globalOS.mousePosition.x - lastMouse.x, globalOS.mousePosition.y - lastMouse.y);
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
        return 1;
    }

    HWND window = CreateWindow("ApplicationWindowClass", WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH,
                               DEFAULT_WINDOW_HEIGHT, NULL, NULL, instance, NULL);

    if (!window)
    {
        return 1;
    }

    char executablePath[256];
    char executableDirectory[256];
    char appDllPath[256];
    char appTempDllPath[256];
    GetModuleFileNameA(0, executablePath, sizeof(executablePath));

    _splitpath_s(executablePath,                                   // full path
                 NULL, 0,                                          // drive
                 executableDirectory, sizeof(executableDirectory), // directory
                 NULL, 0,                                          // filename
                 NULL, 0                                           // file extension
    );

    wsprintf(appDllPath, "%s%s.dll", executableDirectory, PROGRAM_FILENAME);
    wsprintf(appTempDllPath, "%s%s_temp.dll", executableDirectory, PROGRAM_FILENAME);

    w32_app_code appCode = {0};
    if (!W32_AppCodeLoad(&appCode, appDllPath, appTempDllPath))
    {
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

    globalOS.sampleOut = VirtualAlloc(0, soundOutput.samplesPerSecond * sizeof(f32) * 2,
                                      MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    globalOS.samplesPerSecond = soundOutput.samplesPerSecond;

    globalOS.Reserve = W32_Reserve;
    globalOS.Release = W32_Release;
    globalOS.Commit = W32_Commit;
    globalOS.Decommit = W32_Decommit;
    globalOS.DebugPrint = W32_DebugPrint;
    globalOS.GLSwapBuffers = W32_GLSwapBuffers;

    globalOS.permanentArena = MemoryArenaInitialize(Gigabytes(4));
    globalOS.frameArena = MemoryArenaInitialize(Gigabytes(4));

    globalHDC = GetDC(window);
    HGLRC glContext = W32_InitOpenGL(globalHDC);

    W32_LoadXInput();

    appCode.PermanentLoad(&globalOS);
    appCode.HotLoad(&globalOS);

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

        appCode.Update();

        if (soundOutput.initialized)
        {
            W32_FillSoundBuffer(globalOS.sampleCount, globalOS.sampleOut, &soundOutput);
        }

        W32_AppCodeMaybeHotLoad(&appCode, appDllPath, appTempDllPath);
        W32_TimerEndFrame(&timer);
    }

    ShowWindow(window, SW_HIDE);
    W32_AppCodeUnload(&appCode);
    W32_CleanUpOpenGL(&globalHDC, glContext);

    return 0;
}
