#include "geometry.h"
#include "gl.h"
#include "input.h"
#include "win32_opengl.h"
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define W32_WNDCLASSNAME "ApplicationWindowClass"

static struct
{
    HWND handle;
    HDC hdc;
} g_Internal;

WindowState window;

static i32 W32_VKeyCodeToInputKey(u64 vkeyCode)
{
    if (vkeyCode >= 'A' && vkeyCode <= 'Z')
    {
        return (i32)(Key_A + vkeyCode - 'A');
    }

    if (vkeyCode >= '0' && vkeyCode <= '9')
    {
        return (i32)(Key_0 + vkeyCode - '0');
    }

    if (vkeyCode >= VK_F1 && vkeyCode <= VK_F12)
    {
        return (i32)(Key_F1 + vkeyCode - VK_F1);
    }

    switch (vkeyCode)
    {
    case VK_ESCAPE:
        return Key_Esc;
    case VK_OEM_3:
        return Key_Grave;
    case VK_OEM_MINUS:
        return Key_Minus;
    case VK_OEM_PLUS:
        return Key_Equal;
    case VK_BACK:
        return Key_Backspace;
    case VK_TAB:
        return Key_Tab;
    case VK_SPACE:
        return Key_Space;
    case VK_RETURN:
        return Key_Enter;
    case VK_CONTROL:
        return Key_LeftCtrl;
    case VK_SHIFT:
        return Key_LeftShift;
    case VK_MENU:
        return Key_LeftAlt;
    case VK_UP:
        return Key_Up;
    case VK_LEFT:
        return Key_Left;
    case VK_DOWN:
        return Key_Down;
    case VK_RIGHT:
        return Key_Right;
    case VK_DELETE:
        return Key_Delete;
    case VK_PRIOR:
        return Key_PageUp;
    case VK_NEXT:
        return Key_PageDown;
    case VK_HOME:
        return Key_Home;
    case VK_END:
        return Key_End;
    case VK_OEM_2:
        return Key_ForwardSlash;
    case VK_OEM_PERIOD:
        return Key_Period;
    case VK_OEM_COMMA:
        return Key_Comma;
    case VK_OEM_7:
        return Key_Quote;
    case VK_OEM_4:
        return Key_LeftBracket;
    case VK_OEM_6:
        return Key_RightBracket;
    default:
        return Key_Unknown;
    }
}

static LRESULT CALLBACK W32_WindowProcedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
        window.quit = true;
        return 0;
    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);
    case WM_SIZE:
        window.width = LOWORD(lParam);
        window.height = HIWORD(lParam);
        glViewport(0, 0, window.width, window.height);
        return 0;
    case WM_KEYDOWN: {
        b32 prevDown = lParam & (1 << 30);
        (prevDown ? OnKeyRepeat : OnKeyPress)(W32_VKeyCodeToInputKey(wParam));
        return 0;
    }
    case WM_KEYUP:
        OnKeyRelease(W32_VKeyCodeToInputKey(wParam));
        return 0;
    case WM_LBUTTONDOWN:
        OnMouseClick(Mouse_Left);
        return 0;
    case WM_LBUTTONUP:
        OnMouseRelease(Mouse_Left);
        return 0;
    case WM_RBUTTONDOWN:
        OnMouseClick(Mouse_Right);
        return 0;
    case WM_RBUTTONUP:
        OnMouseRelease(Mouse_Right);
        return 0;
    case WM_MOUSEMOVE: {
        POINT mouse;
        GetCursorPos(&mouse);
        ScreenToClient(handle, &mouse);
        OnMouseMove(mouse.x, mouse.y);
        return 0;
    }
    default:
        return DefWindowProc(handle, message, wParam, lParam);
    }
}

b32 WindowCreate(i32 width, i32 height, const char *title)
{
    WNDCLASS windowClass = {0};
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = W32_WindowProcedure;
    windowClass.hInstance = GetModuleHandleW(NULL);
    windowClass.lpszClassName = W32_WNDCLASSNAME;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&windowClass))
    {
        return false;
    }

    DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    RECT windowSize;
    windowSize.left = 0;
    windowSize.top = 0;
    windowSize.right = width;
    windowSize.bottom = height;

    if (!AdjustWindowRect(&windowSize, windowStyle, 0))
    {
        return false;
    }

    HWND handle =
        CreateWindow(W32_WNDCLASSNAME, title, windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                     windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, NULL,
                     NULL, GetModuleHandleW(NULL), NULL);

    if (!handle)
    {
        return false;
    }

    HDC hdc = GetDC(handle);
    if (!W32_InitOpenGL(hdc))
    {
        return false;
    }
    LoadOpenGLProcs(W32_GetProcAddress);

    ShowWindow(handle, SW_SHOW);

    g_Internal.handle = handle;
    g_Internal.hdc = hdc;

    window.quit = false;

    return true;
}

void *WindowGetHandle(void)
{
    return g_Internal.handle;
}

b32 WindowIsFocused(void)
{
    return g_Internal.handle == GetFocus();
}

void WindowPollEvents(void)
{
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void WindowSwapInterval(i32 interval)
{
    wglSwapIntervalEXT(interval);
}

void WindowSwapBuffers(void)
{
    SwapBuffers(g_Internal.hdc);
}

v4 WindowRect(void)
{
    RECT rect;
    GetWindowRect(g_Internal.handle, &rect);
    return v4((f32)rect.left, (f32)rect.top, (f32)rect.right, (f32)rect.bottom);
}
