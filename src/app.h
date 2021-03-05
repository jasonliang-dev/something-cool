typedef struct AppState AppState;
struct AppState
{
    SDL_Window *window;
    SDL_GLContext glContext;
    struct nk_context *nkContext;

    Audio audio;
    Renderer renderer;

    i32 windowWidth;
    i32 windowHeight;

    // should this be volatile?
    b32 running;

    i32 keyCount;
    const u8 *keyDown;
    const u8 *keyDownPrev;
    f32 deltaTime;

    b32 showOverview;

    Texture dog;

    /*
    SpriteAnimation boy;
    Tilemap map;
    Sound wobble;
    Sound coin;
    */

#define X(name, file, width, msPerFrame) SpriteAnimation name;
    SPRITE_ANIMATION_ASSETS
#undef X

#define X(name, atlas, data) Tilemap name;
    TILEMAP_ASSETS
#undef X

#define X(name, file) Sound name;
    SOUND_ASSETS
#undef X
};
