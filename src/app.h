struct Texture
{
    SDL_Texture *texture;
    i32 width;
    i32 height;
};

struct Tilemap
{
    Texture atlas;
    i32 tileWidth; // in pixels. width == height
    i32 width;
    i32 height;
    i32 *indices;
    u8 *collision;
};

struct Entity
{
    v2 position;
    v2 velocity;
    v2 acceleration;
    Texture image;
};

struct AppState
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    // should this be volatile?
    bool running;

    bool debug;
    i32 keyCount;
    const u8 *keyDown;
    const u8 *keyDownPrev;
    f32 deltaTime;
    i32 scale;

    v2 camera;
    TTF_Font *fntSans;
    Entity player;
    Tilemap map;
};
