#define WIN32_LEAN_AND_MEAN

#include "os.h"
#include <windows.h>

i32 GetSystemPageSize(void)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

void *MemReserve(size_t size)
{
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
}

void MemCommit(void *ptr, size_t size)
{
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

void MemFree(void *ptr, size_t size)
{
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}
