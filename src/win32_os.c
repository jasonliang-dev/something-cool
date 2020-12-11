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

internal void OS_DebugPrint(char *format, ...)
{
    char buff[2048];

    va_list args;
    va_start(args, format);
    vsnprintf_s(buff, ArrayCount(buff), _TRUNCATE, format, args);
    va_end(args);

    OutputDebugStringA(buff);
}

internal void OS_DisplayError(char *format, ...)
{
    char buff[2048];

    va_list args;
    va_start(args, format);
    vsnprintf_s(buff, ArrayCount(buff), _TRUNCATE, format, args);
    va_end(args);

    MessageBoxA(NULL, buff, "If you're reading this, then I'm a bad programmer",
                MB_OK | MB_ICONEXCLAMATION);
}

internal void OS_ShowCursor(b32 shown)
{
    if (shown)
    {
        while (ShowCursor(1) < 0)
            ;
    }
    else
    {
        while (ShowCursor(0) >= 0)
            ;
    }
}
