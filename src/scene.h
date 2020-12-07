typedef struct scene_t scene_t;
struct scene_t {
    void (*Init)(memory_arena *arena);
    void (*Destroy)(memory_arena *arena);
    void (*Update)(memory_arena *arena);
};
