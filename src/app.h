typedef struct app_resources_t app_resources_t;
struct app_resources_t
{
    sound_t sndJingle;
    sound_t sndImpact;
    texture_t texPlay;
    texture_t texQuit;
    texture_t texCursor;
    texture_t texBone;
    texture_t texDog;
    texture_t texAtlas;
    tilemap_t map;
};

typedef struct app_state_t app_state_t;
struct app_state_t
{
    b8 keyDown[Key_Max];
    b8 keyPress[Key_Max];
    b8 mouseDown[MouseButton_Max];
    b8 mousePress[MouseButton_Max];

    ui_t ui;
    audio_t audio;

    app_resources_t resources;
    r_renderer_t renderer;

    scene_t scene;
    memory_arena_t sceneArena;
    memory_arena_t scratchArena;
};
