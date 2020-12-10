internal void MenuSceneBegin(memory_arena_t *arena)
{
    menu_scene_t *scene = M_ArenaPushZero(arena, sizeof(menu_scene_t));
    (void)scene;
}

internal void MenuSceneEnd(void *memory)
{
    (void)memory;
}

internal b32 MenuSceneUpdate(void *memory, scene_t *nextScene)
{
    menu_scene_t *scene = memory;
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
