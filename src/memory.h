typedef struct memory_arena memory_arena;
struct memory_arena
{
    void *base;
    u64 max;
    u64 allocPosition;
    u64 commitPosition;
};
