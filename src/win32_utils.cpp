internal v2 W32_GetMousePosition(HWND window)
{
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window, &mouse);
    return v2((f32)mouse.x, (f32)mouse.y);
}

internal void W32_ToggleFullscreen(HWND window)
{
    persistent WINDOWPLACEMENT lastWindowPlacement = {sizeof(lastWindowPlacement)};

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
        *osKey = (vkeyCode >= 'A' && vkeyCode <= 'Z') ? OS_Key_A + (vkeyCode - 'A')
                                                      : OS_Key_0 + (vkeyCode - '0');
    }
    else if (vkeyCode >= VK_F1 && vkeyCode <= VK_F12)
    {
        *osKey = OS_Key_F1 + vkeyCode - VK_F1;
    }
    else if (vkeyCode == VK_ESCAPE)
    {
        *osKey = OS_Key_Esc;
    }
    else if (vkeyCode == VK_OEM_3)
    {
        *osKey = OS_Key_GraveAccent;
    }
    else if (vkeyCode == VK_OEM_MINUS)
    {
        *osKey = OS_Key_Minus;
    }
    else if (vkeyCode == VK_OEM_PLUS)
    {
        *osKey = OS_Key_Equal;
    }
    else if (vkeyCode == VK_BACK)
    {
        *osKey = OS_Key_Backspace;
    }
    else if (vkeyCode == VK_TAB)
    {
        *osKey = OS_Key_Tab;
    }
    else if (vkeyCode == VK_SPACE)
    {
        *osKey = OS_Key_Space;
    }
    else if (vkeyCode == VK_RETURN)
    {
        *osKey = OS_Key_Enter;
    }
    else if (vkeyCode == VK_CONTROL)
    {
        *osKey = OS_Key_Ctrl;
        *modifiers &= ~OS_KeyModifier_Ctrl;
    }
    else if (vkeyCode == VK_SHIFT)
    {
        *osKey = OS_Key_Shift;
        *modifiers &= ~OS_KeyModifier_Shift;
    }
    else if (vkeyCode == VK_MENU)
    {
        *osKey = OS_Key_Alt;
        *modifiers &= ~OS_KeyModifier_Alt;
    }
    else if (vkeyCode == VK_UP)
    {
        *osKey = OS_Key_Up;
    }
    else if (vkeyCode == VK_LEFT)
    {
        *osKey = OS_Key_Left;
    }
    else if (vkeyCode == VK_DOWN)
    {
        *osKey = OS_Key_Down;
    }
    else if (vkeyCode == VK_RIGHT)
    {
        *osKey = OS_Key_Right;
    }
    else if (vkeyCode == VK_DELETE)
    {
        *osKey = OS_Key_Delete;
    }
    else if (vkeyCode == VK_PRIOR)
    {
        *osKey = OS_Key_PageUp;
    }
    else if (vkeyCode == VK_NEXT)
    {
        *osKey = OS_Key_PageDown;
    }
    else if (vkeyCode == VK_HOME)
    {
        *osKey = OS_Key_Home;
    }
    else if (vkeyCode == VK_END)
    {
        *osKey = OS_Key_End;
    }
    else if (vkeyCode == VK_OEM_2)
    {
        *osKey = OS_Key_ForwardSlash;
    }
    else if (vkeyCode == VK_OEM_PERIOD)
    {
        *osKey = OS_Key_Period;
    }
    else if (vkeyCode == VK_OEM_COMMA)
    {
        *osKey = OS_Key_Comma;
    }
    else if (vkeyCode == VK_OEM_7)
    {
        *osKey = OS_Key_Quote;
    }
    else if (vkeyCode == VK_OEM_4)
    {
        *osKey = OS_Key_LeftBracket;
    }
    else if (vkeyCode == VK_OEM_6)
    {
        *osKey = OS_Key_RightBracket;
    }
}
