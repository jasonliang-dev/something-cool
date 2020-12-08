internal void MenuSceneInit(memory_arena_t *arena)
{
    menu_scene_t *scene = M_ArenaPushZero(arena, sizeof(menu_scene_t));
    (void)scene;
}

internal void MenuSceneDestroy(menu_scene_t *scene)
{
    (void)scene;
}

internal b32 MenuSceneUpdate(menu_scene_t *scene, scene_t *nextScene)
{
    (void)scene;

    if (app.keyPress[Key_Esc])
    {
        *nextScene = Scene_CreateFrom(Game);
        return 1;
    }

    return 0;
}
