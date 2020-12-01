internal void *W32_Reserve(u64 size)
{
    void *memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return memory;
}

internal void W32_Release(void *memory, u64 size)
{
    (void)size;
    VirtualFree(memory, 0, MEM_RELEASE);
}

internal void W32_Commit(void *memory, u64 size)
{
    VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}

internal void W32_Decommit(void *memory, u64 size)
{
    VirtualFree(memory, size, MEM_DECOMMIT);
}

internal void W32_DebugPrint(char *str)
{
    OutputDebugStringA(str);
}

internal void W32_DisplayError(char *str)
{
    MessageBoxA(NULL, str, "Error", MB_OK | MB_ICONEXCLAMATION);
}

internal v2 W32_GetMousePosition(HWND window)
{
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window, &mouse);
    return v2((f32)mouse.x, (f32)mouse.y);
}
