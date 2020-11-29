#define MEMORY_ARENA_COMMIT_SIZE Kilobytes(4)

internal memory_arena M_ArenaInitialize(u64 cap)
{
    memory_arena arena = {0};
    arena.max = cap;
    arena.base = os->Reserve(arena.max);
    arena.allocPosition = 0;
    arena.commitPosition = 0;
    return arena;
}

internal void *M_ArenaPush(memory_arena *arena, u64 size)
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

internal void *M_ArenaPushZero(memory_arena *arena, u64 size)
{
    void *memory = M_ArenaPush(arena, size);
    MemorySet(memory, 0, size);
    return memory;
}

internal void M_ArenaPop(memory_arena *arena, u64 size)
{
    if (size > arena->allocPosition)
    {
        size = arena->allocPosition;
    }

    arena->allocPosition -= size;
}

internal void M_ArenaClear(memory_arena *arena)
{
    M_ArenaPop(arena, arena->allocPosition);
}

internal void M_ArenaRelease(memory_arena *arena)
{
    os->Release(arena->base);
}
