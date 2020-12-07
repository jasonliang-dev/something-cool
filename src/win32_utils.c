internal void *OS_Reserve(u64 size)
{
    void *memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return memory;
}

internal void OS_Release(void *memory, u64 size)
{
    (void)size;
    VirtualFree(memory, 0, MEM_RELEASE);
}

internal void OS_Commit(void *memory, u64 size)
{
    VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}

internal void OS_Decommit(void *memory, u64 size)
{
    VirtualFree(memory, size, MEM_DECOMMIT);
}

internal void OS_DebugPrint(char *str)
{
    OutputDebugStringA(str);
}

internal void OS_DisplayError(char *str)
{
    MessageBoxA(NULL, str, "Error", MB_OK | MB_ICONEXCLAMATION);
}

internal void OS_ShowCursor(b32 shown)
{
    ShowCursor(shown);
}

internal v2 W32_GetMousePosition(HWND window)
{
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window, &mouse);
    return v2((f32)mouse.x, (f32)mouse.y);
}

internal void W32_ToggleFullscreen(HWND window)
{
    local_persist WINDOWPLACEMENT lastWindowPlacement = {sizeof(lastWindowPlacement)};

    DWORD windowStyle = GetWindowLong(window, GWL_STYLE);
    if (windowStyle & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitorInfo = {sizeof(monitorInfo)};
        if (GetWindowPlacement(window, &lastWindowPlacement) &&
            GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
        {
            SetWindowLong(window, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);

            SetWindowPos(window, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                         monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                         monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(window, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &lastWindowPlacement);
        SetWindowPos(window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal void W32_KeyCodeToOSKey(u64 vkeyCode, u64 *osKey, u32 *modifiers)
{
    if ((vkeyCode >= 'A' && vkeyCode <= 'Z') || (vkeyCode >= '0' && vkeyCode <= '9'))
    {
        *osKey = (vkeyCode >= 'A' && vkeyCode <= 'Z') ? Key_A + (vkeyCode - 'A')
                                                        : Key_0 + (vkeyCode - '0');
    }
    else if (vkeyCode >= VK_F1 && vkeyCode <= VK_F12)
    {
        *osKey = Key_F1 + vkeyCode - VK_F1;
    }
    else if (vkeyCode == VK_ESCAPE)
    {
        *osKey = Key_Esc;
    }
    else if (vkeyCode == VK_OEM_3)
    {
        *osKey = Key_GraveAccent;
    }
    else if (vkeyCode == VK_OEM_MINUS)
    {
        *osKey = Key_Minus;
    }
    else if (vkeyCode == VK_OEM_PLUS)
    {
        *osKey = Key_Equal;
    }
    else if (vkeyCode == VK_BACK)
    {
        *osKey = Key_Backspace;
    }
    else if (vkeyCode == VK_TAB)
    {
        *osKey = Key_Tab;
    }
    else if (vkeyCode == VK_SPACE)
    {
        *osKey = Key_Space;
    }
    else if (vkeyCode == VK_RETURN)
    {
        *osKey = Key_Enter;
    }
    else if (vkeyCode == VK_CONTROL)
    {
        *osKey = Key_Ctrl;
        *modifiers &= ~KeyModifier_Ctrl;
    }
    else if (vkeyCode == VK_SHIFT)
    {
        *osKey = Key_Shift;
        *modifiers &= ~KeyModifier_Shift;
    }
    else if (vkeyCode == VK_MENU)
    {
        *osKey = Key_Alt;
        *modifiers &= ~KeyModifier_Alt;
    }
    else if (vkeyCode == VK_UP)
    {
        *osKey = Key_Up;
    }
    else if (vkeyCode == VK_LEFT)
    {
        *osKey = Key_Left;
    }
    else if (vkeyCode == VK_DOWN)
    {
        *osKey = Key_Down;
    }
    else if (vkeyCode == VK_RIGHT)
    {
        *osKey = Key_Right;
    }
    else if (vkeyCode == VK_DELETE)
    {
        *osKey = Key_Delete;
    }
    else if (vkeyCode == VK_PRIOR)
    {
        *osKey = Key_PageUp;
    }
    else if (vkeyCode == VK_NEXT)
    {
        *osKey = Key_PageDown;
    }
    else if (vkeyCode == VK_HOME)
    {
        *osKey = Key_Home;
    }
    else if (vkeyCode == VK_END)
    {
        *osKey = Key_End;
    }
    else if (vkeyCode == VK_OEM_2)
    {
        *osKey = Key_ForwardSlash;
    }
    else if (vkeyCode == VK_OEM_PERIOD)
    {
        *osKey = Key_Period;
    }
    else if (vkeyCode == VK_OEM_COMMA)
    {
        *osKey = Key_Comma;
    }
    else if (vkeyCode == VK_OEM_7)
    {
        *osKey = Key_Quote;
    }
    else if (vkeyCode == VK_OEM_4)
    {
        *osKey = Key_LeftBracket;
    }
    else if (vkeyCode == VK_OEM_6)
    {
        *osKey = Key_RightBracket;
    }
}
