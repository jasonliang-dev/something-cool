struct app_resources_t
{
    sound_t sndJingle;
    sound_t sndImpact;
    font_t fntFont;
    texture_t texPlay;
    texture_t texQuit;
    texture_t texCursor;
    texture_t texBone;
    texture_t texDog;
    texture_t texAtlas;
    tilemap_t map;
};

struct app_state_t
{
    b8 keyDown[OS_Key_Max];
    b8 keyPress[OS_Key_Max];
    b8 mouseDown[OS_MouseButton_Max];
    b8 mousePress[OS_MouseButton_Max];

    ui_t ui;
    audio_t audio;

    app_resources_t resources;
    renderer_t renderer;
    b32 isWireframe;

    scene_t scene;
    memory_arena_t permanentArena;
    memory_arena_t sceneArena;
    memory_arena_t scratchArena;
};
