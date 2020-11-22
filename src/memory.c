#define MEMORY_ARENA_MAX Gigabytes(4)
#define MEMORY_ARENA_COMMIT_SIZE Kilobytes(4)

internal memory_arena MemoryArenaInitialize(void)
{
    memory_arena arena = {0};
    arena.max = MEMORY_ARENA_MAX;
    arena.base = os->Reserve(arena.max);
    arena.allocPosition = 0;
    arena.commitPosition = 0;
    return arena;
}

internal void *MemoryArenaPush(memory_arena *arena, u64 size)
{
    void *memory = 0;

    if (arena->allocPosition + size > arena->commitPosition)
    {
        u64 commitSize = size;
        commitSize += MEMORY_ARENA_COMMIT_SIZE - 1;
        commitSize -= commitSize % MEMORY_ARENA_COMMIT_SIZE;
        os->Commit((u8 *)arena->base + arena->commitPosition, commitSize);
        arena->commitPosition += commitSize;
    }

    memory = (u8 *)arena->base + arena->allocPosition;
    arena->allocPosition += size;
    return memory;
}

internal void *MemoryArenaPushZero(memory_arena *arena, u64 size)
{
    void *memory = MemoryArenaPush(arena, size);
    MemorySet(memory, 0, size);
    return memory;
}

internal void MemoryArenaPop(memory_arena *arena, u64 size)
{
    if (size > arena->allocPosition)
    {
        size = arena->allocPosition;
    }

    arena->allocPosition -= size;
}

internal void MemoryArenaClear(memory_arena *arena)
{
    MemoryArenaPop(arena, arena->allocPosition);
}

internal void MemoryArenaRelease(memory_arena *arena)
{
    os->Release(arena->base);
}
