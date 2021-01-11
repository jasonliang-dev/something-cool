#define MEMORY_ARENA_COMMIT_SIZE Kilobytes(4)

internal memory_arena_t Memory_ArenaInitialize(u64 cap)
{
    memory_arena_t arena = {0};
    arena.max = cap;
    arena.base = OS_Reserve(arena.max);
    arena.allocPosition = 0;
    arena.commitPosition = 0;
    return arena;
}

internal void *Memory_ArenaPush(memory_arena_t *arena, u64 size)
{
    void *memory = 0;

    if (arena->allocPosition + size > arena->commitPosition)
    {
        u64 commitSize = size;
        commitSize += MEMORY_ARENA_COMMIT_SIZE - 1;
        commitSize -= commitSize % MEMORY_ARENA_COMMIT_SIZE;
        OS_Commit((u8 *)arena->base + arena->commitPosition, commitSize);
        arena->commitPosition += commitSize;
    }

    memory = (u8 *)arena->base + arena->allocPosition;
    arena->allocPosition += size;
    return memory;
}

internal void *Memory_ArenaPushZero(memory_arena_t *arena, u64 size)
{
    void *memory = Memory_ArenaPush(arena, size);
    MemorySet(memory, 0, size);
    return memory;
}

internal void Memory_ArenaPop(memory_arena_t *arena, u64 size)
{
    if (size > arena->allocPosition)
    {
        size = arena->allocPosition;
    }

    arena->allocPosition -= size;
}

internal void Memory_ArenaClear(memory_arena_t *arena)
{
    Memory_ArenaPop(arena, arena->allocPosition);
}

internal void Memory_ArenaRelease(memory_arena_t *arena)
{
    OS_Release(arena->base, arena->max);
}
