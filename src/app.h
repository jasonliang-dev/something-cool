typedef struct app_resources_t app_resources_t;
struct app_resources_t
{
    texture_t face;
    texture_t cursor;
    texture_t bone;
    texture_t dog;
    texture_t atlas;
    tilemap_t map;
};

typedef struct app_state_t app_state_t;
struct app_state_t
{
    b8 keysDown[Key_Max];
    b8 mouseDown[MouseButton_Max];

    u32 scale;
    u32 spriteShader;
    u32 mapShader;
    u32 vao;
    app_resources_t resources;

    scene_t scene;
    memory_arena sceneArena;
    memory_arena scratchArena;
};
