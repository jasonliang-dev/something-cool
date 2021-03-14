internal void *MemAlloc(size_t size)
{
    void *mem = malloc(size);
    if (!mem)
    {
        abort();
    }
    return mem;
}

internal void *MemRealloc(void *mem, size_t size)
{
    void *newMem = realloc(mem, size);
    if (!newMem)
    {
        abort();
    }

    return newMem;
}
