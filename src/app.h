struct AppState
{
    SDL_Window *window;
    SDL_GLContext glContext;
    struct nk_context *nkContext;

    Renderer renderer;
    Texture dog;
    Tilemap map;

    i32 windowWidth;
    i32 windowHeight;

    // should this be volatile?
    b32 running;

    i32 keyCount;
    const u8 *keyDown;
    const u8 *keyDownPrev;
    f32 deltaTime;

    b32 showImguiDemoWindow;
};
