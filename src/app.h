struct Texture
{
    SDL_Texture *texture;
    i32 width;
    i32 height;
};

struct Tilemap
{
    Texture atlas;
    i32 tileSize; // width and height in pixels
    i32 width;
    i32 height;
    i32 *tiles;
};

struct AppState
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    // should this be volatile?
    bool running;

    i32 keyCount;
    const u8 *keyDown;
    u8 *keyDownPrev;
    f32 deltaTime;
    i32 scale;

    v2 camera;
    Texture texDog;
    Tilemap map;
};
