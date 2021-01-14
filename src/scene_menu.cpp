internal void MenuSceneBegin(memory_arena_t *arena)
{
    menu_scene_t *scene = (menu_scene_t *)Memory_ArenaPushZero(arena, sizeof(menu_scene_t));
    (void)scene;

    OS_ShowCursor(true);
}

internal void MenuSceneEnd(void *memory)
{
    (void)memory;
}

internal b32 MenuScenePixelUpdate(void *memory, scene_t *nextScene)
{
    b32 result = false;

    menu_scene_t *scene = (menu_scene_t *)memory;
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

internal b32 MenuSceneNativeUpdate(void *memory, scene_t *nextScene)
{
    (void)memory;
    (void)nextScene;

    UI_BeginFrame(&app->ui);
    {
        UI_PushFlex(&app->ui, v2(50, 50), v2(256, 64), UI_FLEX_COLUMN);
        {
            if (UI_Button(&app->ui, UI_AutoID(), "Sample Text"))
            {
                OS_DebugPrint("Clicked\n");
            }

            persistent f32 slider = 0.5f;

            slider = UI_Slider(&app->ui, UI_AutoID(), slider);
        }
        UI_PopFlex(&app->ui);
    }
    UI_EndFrame(&app->ui);

    Render_DrawText(&app->resources.fntFont, v2(0, 0),
                    "The quick brown fox jumped over the lazy dog.|");

    return false;
}
