struct AppState
{
    SDL_Window *window;
    SDL_GLContext glContext;

    Renderer renderer;
    Texture dog;

    i32 windowWidth;
    i32 windowHeight;

    // should this be volatile?
    b32 running;

    i32 keyCount;
    const u8 *keyDown;
    const u8 *keyDownPrev;
    f32 deltaTime;

    bool showImguiDemoWindow;
};
