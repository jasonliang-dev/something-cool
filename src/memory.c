#include "memory.h"
#include "os.h"
#include <assert.h>
#include <string.h>

static struct
{
    MemoryArena permanent;
    MemoryArena scratch;
} g_Memory;

MemoryArena CreateArena(size_t capacity)
{
    MemoryArena result;

    result.buffer = MemReserve(capacity);
    result.allocd = result.buffer;
    result.committed = result.buffer;
    result.end = result.buffer + capacity;

    return result;
}

static b32 IsPowerOfTwo(size_t x)
{
    return (x & (x - 1)) == 0;
}

static u8 *AlignForward(u8 *ptr, size_t align)
{
    assert(IsPowerOfTwo(align));
    uintptr_t modulo = (uintptr_t)ptr & (align - 1);

    if (modulo != 0)
    {
        ptr += align - modulo;
    }

    return ptr;
}

void *ArenaAlloc(MemoryArena *a, size_t size, size_t align)
{
    u8 *aligned = AlignForward(a->allocd, align);
    assert(aligned + size <= a->end);

    if (aligned + size > a->committed)
    {
        enum
        {
            PAGE_SIZE = Kilobytes(4)
        };

        size_t commitSize = size + (PAGE_SIZE - 1);
        commitSize -= commitSize % PAGE_SIZE;
        MemCommit(a->committed, commitSize);
        a->committed += commitSize;
    }

    a->allocd = aligned + size;
    return aligned;
}

void ArenaReset(MemoryArena *a)
{
    a->allocd = a->buffer;
}

void ArenaFree(MemoryArena *a)
{
    MemFree(a->buffer, (a->buffer - a->end));
    memset(a, 0, sizeof(MemoryArena));
}

void InitMemoryArenas(void)
{
    assert(GetSystemPageSize() == Kilobytes(4));

    g_Memory.permanent = CreateArena(Gigabytes(4));
    g_Memory.scratch = CreateArena(Gigabytes(4));
}

void *PermanentAlloc(size_t size)
{
    return ArenaAlloc(&g_Memory.permanent, size, 16);
}

void *PermanentAllocAlign(size_t size, size_t align)
{
    return ArenaAlloc(&g_Memory.permanent, size, align);
}

void *ScratchAlloc(size_t size)
{
    return ArenaAlloc(&g_Memory.scratch, size, 16);
}

void *ScratchAllocAlign(size_t size, size_t align)
{
    return ArenaAlloc(&g_Memory.scratch, size, align);
}

void ScratchReset(void)
{
    ArenaReset(&g_Memory.scratch);
}

ArenaSavePoint ArenaSave(MemoryArena *a)
{
    ArenaSavePoint result;
    result.arena = a;
    result.saved = a->allocd;
    return result;
}

void ArenaRestore(ArenaSavePoint s)
{
    s.arena->allocd = s.saved;
}
