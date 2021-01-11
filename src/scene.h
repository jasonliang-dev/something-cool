struct scene_t
{
    void (*Begin)(memory_arena_t *arena);
    void (*End)(void *memory);
    b32 (*PixelUpdate)(void *memory, scene_t *nextScene);
    b32 (*NativeUpdate)(void *memory, scene_t *nextScene);
};

#define SceneCreate(name)                                                                          \
    (scene_t{name##SceneBegin, name##SceneEnd, name##ScenePixelUpdate, name##SceneNativeUpdate});
