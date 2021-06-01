#define WIN32_LEAN_AND_MEAN

#include "os.h"
#include <windows.h>

i32 GetSystemPageSize(void)
{
    static i32 s_PageSize = 0;

    if (s_PageSize == 0)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        s_PageSize = info.dwPageSize;
    }

    return s_PageSize;
}

void *MemReserve(size_t size)
{
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
}

void MemCommit(void *ptr, size_t size)
{
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

void MemFree(void *ptr)
{
    VirtualFree(ptr, 0, MEM_RELEASE);
}
