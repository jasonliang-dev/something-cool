typedef struct scene_t scene_t;
struct scene_t
{
    void (*Begin)(memory_arena_t *arena);
    void (*End)(void *memory);
    b32 (*Update)(void *memory, scene_t *nextScene);
};

#define SceneCreate(name)                                                                          \
    (scene_t{name##SceneBegin, name##SceneEnd, name##SceneUpdate});
