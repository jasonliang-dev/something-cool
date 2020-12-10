typedef struct menu_scene_t menu_scene_t;
struct menu_scene_t
{
    u32 empty;
};

internal void MenuSceneBegin(memory_arena_t *arena);
internal void MenuSceneEnd(void *memory);
internal b32 MenuSceneUpdate(void *memory, scene_t *nextScene);
