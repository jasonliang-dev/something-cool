internal void *OS_Reserve(u64 size)
{
    void *memory = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    msync(memory, size, MS_SYNC | MS_INVALIDATE);
    return memory;
}

internal void OS_Release(void *memory, u64 size)
{
    msync(memory, size, MS_SYNC);
    munmap(memory, size);
}

internal void OS_Commit(void *memory, u64 size)
{
    mmap(memory, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    msync(memory, size, MS_SYNC | MS_INVALIDATE);
}

internal void OS_Decommit(void *memory, u64 size)
{
    mmap(memory, size, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    msync(memory, size, MS_SYNC | MS_INVALIDATE);
}

internal void OS_DebugPrint(char *str)
{
    fputs(str, stdout);
}

internal void OS_DisplayError(char *str)
{
    fputs(str, stderr);
}

internal void OS_ShowCursor(b32 shown)
{
    (void)shown;
}
