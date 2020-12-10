internal void MenuSceneBegin(memory_arena_t *arena)
{
    menu_scene_t *scene = M_ArenaPushZero(arena, sizeof(menu_scene_t));
    (void)scene;
}

internal void MenuSceneEnd(menu_scene_t *scene)
{
    (void)scene;
}

internal b32 MenuSceneUpdate(menu_scene_t *scene, scene_t *nextScene)
{
    (void)scene;

    R_DrawSprite(app->resources.play, v2(LOW_RES_SCREEN_WIDTH / 2.0f, 64.0f), 0);
    R_DrawSprite(app->resources.quit, v2(LOW_RES_SCREEN_WIDTH / 2.0f, 128.0f), 0);

    if (app->keyPress[Key_Esc])
    {
        *nextScene = SceneCreate(Game);
        return 1;
    }

    return 0;
}
