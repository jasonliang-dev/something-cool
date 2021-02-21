struct AppState
{
    SDL_Window *window;
    SDL_GLContext glContext;

    Renderer renderer;
    Texture dog;

    // should this be volatile?
    bool running;

    bool debug;
    i32 keyCount;
    const u8 *keyDown;
    const u8 *keyDownPrev;
    f32 deltaTime;
};
