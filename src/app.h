typedef struct app_resources_t app_resources_t;
struct app_resources_t
{
    texture_t play;
    texture_t quit;
    texture_t cursor;
    texture_t bone;
    texture_t dog;
    texture_t atlas;
    tilemap_t map;
};

typedef struct app_shaders_t app_shaders_t;
struct app_shaders_t {
    u32 sprite;
    u32 map;
};

typedef struct app_state_t app_state_t;
struct app_state_t
{
    b8 keyDown[Key_Max];
    b8 keyPress[Key_Max];
    b8 mouseDown[MouseButton_Max];
    b8 mousePress[MouseButton_Max];

    v2 screenScale;
    u32 quadVAO;
    u32 screenFBO;
    app_resources_t resources;
    app_shaders_t shaders;

    scene_t scene;
    memory_arena_t sceneArena;
    memory_arena_t scratchArena;
};
