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

    ui_input_t uiInput;
    uiInput.cursor = GetCursorPosition();
    uiInput.leftDown = app->mouseDown[MouseButton_Left];
    uiInput.rightDown = app->mouseDown[MouseButton_Right];

    UI_BeginFrame(&app->ui, &uiInput);

    if (UI_ButtonExt(&app->ui, UI_MakeID(1, 1), "Sample Text", v4(32, 32, 120, 32)))
    {
        OS_DebugPrint("Clicked\n");
    }

    persistent f32 slider = 0.5f;

    slider = UI_SliderExt(&app->ui, UI_MakeID(2, 1), slider, v4(32, 128, 120, 16));

    UI_EndFrame(&app->ui);

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
