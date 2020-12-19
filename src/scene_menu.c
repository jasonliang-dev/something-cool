internal void MenuSceneBegin(memory_arena_t *arena)
{
    menu_scene_t *scene = M_ArenaPushZero(arena, sizeof(menu_scene_t));
    (void)scene;

    OS_ShowCursor(true);
}

internal void MenuSceneEnd(void *memory)
{
    (void)memory;
}

internal b32 MenuSceneUpdate(void *memory, scene_t *nextScene)
{
    b32 result = false;

    menu_scene_t *scene = memory;
    (void)scene;

    if (UI_SpriteButton(app->resources.texPlay,
                        v2((LOW_RES_SCREEN_WIDTH - app->resources.texPlay.width) / 2.0f, 32.0f)))
    {
        *nextScene = SceneCreate(Game);
        Audio_PlaySound(&app->audio, &app->resources.sndJingle);
        result = true;
    }

    if (UI_SpriteButton(app->resources.texQuit,
                        v2((LOW_RES_SCREEN_WIDTH - app->resources.texQuit.width) / 2.0f, 160.0f)))
    {
        os->running = false;
    }

    return result;
}
