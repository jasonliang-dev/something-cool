internal void *W32_Reserve(u64 size)
{
    void *memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return memory;
}

internal void W32_Release(void *memory)
{
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
