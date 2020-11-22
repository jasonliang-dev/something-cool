#include <windows.h>

#include "include.c"

global os_state globalOS;

#include "w32_app_code.c"
#include "w32_os_utils.c"
#include "w32_xinput.c"

internal LRESULT CALLBACK W32_WindowProcedure(HWND window, UINT message, WPARAM wParam,
                                              LPARAM lParam)
{
    if (message == WM_CLOSE || message == WM_DESTROY || message == WM_QUIT)
    {
        globalOS.quit = 1;
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

    WNDCLASS windowClass = {0};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
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

    w32_app_code w32AppCode = {0};
    if (!W32_AppCodeLoad(&w32AppCode, appDllPath, appTempDllPath))
    {
        return 1;
    }

    os = &globalOS;
    globalOS.DebugPrint = W32_DebugPrint;

    W32_LoadXInput();

    w32AppCode.PermanentLoad(&globalOS);
    w32AppCode.HotLoad(&globalOS);

    ShowWindow(window, commandShow);
    UpdateWindow(window);

    MSG message;
    while (!globalOS.quit)
    {
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                globalOS.quit = 1;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        W32_UpdateXInput();

        if (globalGamepads[0].connected)
        {
            OutputDebugStringA("connected\n");

            if (globalGamepads[0].buttonStates[GamepadButton_A])
            {
                OutputDebugStringA("A button pressed\n");
            }
        }

        w32AppCode.Update();

        W32_AppCodeHotUpdate(&w32AppCode, appDllPath, appTempDllPath);
    }

    ShowWindow(window, SW_HIDE);
    W32_AppCodeUnload(&w32AppCode);

    return 0;
}
