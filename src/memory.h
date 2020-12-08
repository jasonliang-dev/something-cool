typedef struct memory_arena_t memory_arena_t;
struct memory_arena_t
{
    void *base;
    u64 max;
    u64 allocPosition;
    u64 commitPosition;
};
