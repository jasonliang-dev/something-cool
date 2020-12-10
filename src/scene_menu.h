typedef struct menu_scene_t menu_scene_t;
struct menu_scene_t
{
    u32 empty;
};

internal void MenuSceneBegin(memory_arena_t *arena);
internal void MenuSceneEnd(menu_scene_t *scene);
internal b32 MenuSceneUpdate(menu_scene_t *scene, scene_t *nextScene);
