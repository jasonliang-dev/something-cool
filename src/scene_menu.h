struct menu_scene_t
{
    u32 empty;
};

internal void MenuSceneBegin(memory_arena_t *arena);
internal void MenuSceneEnd(void *memory);
internal b32 MenuScenePixelUpdate(void *memory, scene_t *nextScene);
internal b32 MenuSceneNativeUpdate(void *memory, scene_t *nextScene);
