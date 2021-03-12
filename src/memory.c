internal void *MemAlloc(size_t size)
{
    void *mem = malloc(size);
    if (!mem)
    {
        abort();
    }
    return mem;
}
