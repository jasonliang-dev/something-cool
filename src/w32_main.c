#include <windows.h>

#include "language_layer.c"
#include "program_options.c"
#include "os.c"
#include "w32_xinput.c"

global b32 globalQuit = 0;

internal LRESULT CALLBACK W32_WindowProcedure(HWND window, UINT message, WPARAM wParam,
                                              LPARAM lParam)
{
    if (message == WM_CLOSE || message == WM_DESTROY || message == WM_QUIT)
    {
        globalQuit = 1;
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
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = W32_WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "ApplicationWindowClass";
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&windowClass))
    {
        goto quit;
    }

    HWND window = CreateWindow("ApplicationWindowClass", WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH,
                               DEFAULT_WINDOW_HEIGHT, NULL, NULL, instance, NULL);

    if (!window)
    {
        goto quit;
    }

    ShowWindow(window, commandShow);
    UpdateWindow(window);

    W32_LoadXInput();

    MSG message;
    while (!globalQuit)
    {
        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                globalQuit = 1;
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
    }

quit:;

    return 0;
}
