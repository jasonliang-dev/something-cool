#include <windows.h>

#include "app.h"

global os_state globalOS;
global HDC globalHDC;

#include "w32_app_code.c"
#include "w32_os_utils.c"
#include "w32_dsound.c"
#include "w32_xinput.c"
#include "w32_opengl.c"

internal LRESULT CALLBACK W32_WindowProcedure(HWND window, UINT message, WPARAM wParam,
                                              LPARAM lParam)
{
    if (message == WM_CLOSE || message == WM_DESTROY || message == WM_QUIT)
    {
        globalOS.running = 0;
        return 0;
    }
    else
    {
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
    W32_LoadDirectSound();
    W32_InitDirectSound(window, &soundOutput);
    W32_ClearSoundBuffer(&soundOutput);

    os = &globalOS;
    globalOS.running = 1;

    globalOS.sampleOut =
        VirtualAlloc(0, soundOutput.bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    globalOS.samplesPerSecond = soundOutput.samplesPerSecond;
    globalOS.sampleCount = 0;

    globalOS.Reserve = W32_Reserve;
    globalOS.Release = W32_Release;
    globalOS.Commit = W32_Commit;
    globalOS.Decommit = W32_Decommit;
    globalOS.DebugPrint = W32_DebugPrint;
    globalOS.OpenGLSwapBuffers = W32_OpenGLSwapBuffers;

    globalOS.permanentArena = MemoryArenaInitialize(Gigabytes(4));
    globalOS.frameArena = MemoryArenaInitialize(Gigabytes(4));

    globalHDC = GetDC(window);
    HGLRC glContext = W32_InitOpenGL(globalHDC);

    W32_LoadXInput();

    appCode.PermanentLoad(&globalOS);
    appCode.HotLoad(&globalOS);

    soundOutput.buffer->lpVtbl->Play(soundOutput.buffer, 0, 0, DSBPLAY_LOOPING);

    ShowWindow(window, commandShow);
    UpdateWindow(window);

    MSG message;
    DWORD bytesToLockSoundBuffer;
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

        W32_UpdateXInput();
        W32_FindBytesToLockSoundBuffer(&soundOutput, &bytesToLockSoundBuffer);

        appCode.Update();

        if (soundOutput.initialized)
        {
            W32_FillSoundBuffer(&soundOutput, globalOS.sampleOut, bytesToLockSoundBuffer);
        }

        W32_AppCodeMaybeHotLoad(&appCode, appDllPath, appTempDllPath);
    }

    ShowWindow(window, SW_HIDE);
    W32_AppCodeUnload(&appCode);
    W32_CleanUpOpenGL(&globalHDC, glContext);

    return 0;
}
