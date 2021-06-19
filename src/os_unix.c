#include "os.h"
#include <sys/mman.h>
#include <unistd.h>

i32 GetSystemPageSize(void)
{
    return sysconf(_SC_PAGESIZE);
}

void *MemReserve(size_t size)
{
    void *memory = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    msync(memory, size, MS_SYNC | MS_INVALIDATE);
    return memory;
}

void MemCommit(void *ptr, size_t size)
{
    mprotect(ptr, size, PROT_READ | PROT_WRITE);
    msync(ptr, size, MS_SYNC | MS_INVALIDATE);
}

void MemFree(void *ptr, size_t size)
{
    msync(ptr, size, MS_SYNC);
    munmap(ptr, size);
}
