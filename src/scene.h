typedef struct scene_t scene_t;
struct scene_t
{
    void (*Init)(memory_arena_t *arena);
    void (*Destroy)(void *memory);
    b32 (*Update)(void *memory, scene_t *nextScene);
};

#define Scene_CreateFrom(name)                                                                      \
    ((scene_t){                                                                                    \
        .Init = name##SceneInit, .Destroy = name##SceneDestroy, .Update = name##SceneUpdate});
