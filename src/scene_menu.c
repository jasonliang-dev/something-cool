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

    if (UI_SpriteButton(app->resources.play,
                        v2((LOW_RES_SCREEN_WIDTH - app->resources.play.width) / 2.0f, 32.0f)))
    {
        *nextScene = SceneCreate(Game);
        return 1;
    }

    if (UI_SpriteButton(app->resources.quit,
                        v2((LOW_RES_SCREEN_WIDTH - app->resources.quit.width) / 2.0f, 160.0f)))
    {
        os->running = 0;
    }

    return 0;
}
