#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <SDL.h> // windows
#else
#include <SDL2/SDL.h> // linux
#endif

#include <glad/glad.h>

#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 4505)
#include <stb_image.h>
#pragma warning(pop)

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 4701 4127 4100)
#include <nuklear.h>
#pragma warning(pop)
#define NK_SDL_GL3_IMPLEMENTATION
#include <nuklear_sdl_gl3.h>

#include <glad.c>

#include "language.h"
#include "render.h"
#include "shaders.gen.h"
#include "app.h"

global const i32 NK_MAX_VERTEX_MEMORY = 512 * 1024;
global const i32 NK_MAX_ELEMENT_MEMORY = 128 * 1024;

global const i32 SCREEN_WIDTH = 1366;
global const i32 SCREEN_HEIGHT = 768;

global const i32 PIXEL_ART_SCALE = 4;

global const char *WINDOW_TITLE = "This is a title";

global AppState *app = nullptr;

#include "utils.cpp"
#include "maths.cpp"
#include "gl.cpp"
#include "render.cpp"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // init app
    app = (AppState *)malloc(sizeof(AppState));
    assert(app);
    app->running = true;

    // init sdl
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    i32 glContextFlags = 0;
#ifdef __APPLE__
    // Always required on Mac
    glContextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#endif

#ifdef DEBUG
    glContextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, glContextFlags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    app->windowWidth = SCREEN_WIDTH;
    app->windowHeight = SCREEN_HEIGHT;
    app->window = SDL_CreateWindow(
        WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    assert(app->window);
    app->glContext = SDL_GL_CreateContext(app->window);
    SDL_GL_MakeCurrent(app->window, app->glContext);
    SDL_GL_SetSwapInterval(0); // vsync

    // load opengl procs
    assert(gladLoadGLLoader(SDL_GL_GetProcAddress));

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GL_MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL,
                          GL_FALSE);
#endif

    CreateRenderer(&app->renderer);
    app->dog = CreateTexture("data/dog.png");
    CreateTilemap(&app->map, "data/tiles.png", "data/map.json");

    // keyboard state
    app->keyDown = SDL_GetKeyboardState(&app->keyCount);
    app->keyDownPrev = (const u8 *)malloc(app->keyCount);
    assert(app->keyDownPrev);

    app->nkContext = nk_sdl_init(app->window);
    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();

    v2 cameraPos = v2(0, 0);
    v2 dogPosition = v2(100, 100);
    f32 rotation = 0.0f;
    struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    SDL_Event event;

    u64 counterCurrent = SDL_GetPerformanceCounter();
    u64 counterNew;
    while (app->running)
    {
        counterNew = SDL_GetPerformanceCounter();
        app->deltaTime = (counterNew - counterCurrent) / (f32)SDL_GetPerformanceFrequency();
        counterCurrent = counterNew;

        // app->keyDown updates after event loop.
        // must update keyDownPrev before the loop.
        memcpy((void *)app->keyDownPrev, app->keyDown, app->keyCount);

        nk_input_begin(app->nkContext);
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                app->running = false;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
                    event.window.windowID == SDL_GetWindowID(app->window))
                {
                    app->running = false;
                }
                else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    app->windowWidth = event.window.data1;
                    app->windowHeight = event.window.data2;
                    UpdateProjections(&app->renderer);
                }
            }
            nk_sdl_handle_event(&event);
        }
        nk_input_end(app->nkContext);

        if (nk_begin(app->nkContext, "Window", nk_rect(50, 50, 230, 250),
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                         NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            static int property = 20;

            nk_layout_row_static(app->nkContext, 30, 80, 1);
            if (nk_button_label(app->nkContext, "button"))
            {
                printf("button pressed!\n");
            }
            nk_layout_row_dynamic(app->nkContext, 30, 2);
            nk_layout_row_dynamic(app->nkContext, 22, 1);
            nk_property_int(app->nkContext, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(app->nkContext, 20, 1);
            nk_label(app->nkContext, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(app->nkContext, 25, 1);

            if (nk_combo_begin_color(app->nkContext, nk_rgb_cf(bg),
                                     nk_vec2(nk_widget_width(app->nkContext), 400)))
            {
                nk_layout_row_dynamic(app->nkContext, 120, 1);
                bg = nk_color_picker(app->nkContext, bg, NK_RGBA);
                nk_layout_row_dynamic(app->nkContext, 25, 1);
                bg.r = nk_propertyf(app->nkContext, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(app->nkContext, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(app->nkContext, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(app->nkContext, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(app->nkContext);
            }
        }
        nk_end(app->nkContext);

        glViewport(0, 0, (int)app->windowWidth, (int)app->windowHeight);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        glClear(GL_COLOR_BUFFER_BIT);

        BeginDraw(&app->renderer, cameraPos);
        DrawTilemap(&app->renderer, &app->map);
        DrawTexture(&app->renderer, app->dog, dogPosition, rotation);
        FlushRenderer(&app->renderer);

        nk_sdl_render(NK_ANTI_ALIASING_ON, NK_MAX_VERTEX_MEMORY, NK_MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(app->window);

        GL_CheckForErrors();
    }

    // free all resources
    {
        nk_free(app->nkContext);
        DestroyRenderer(&app->renderer);

        SDL_GL_DeleteContext(app->glContext);
        SDL_DestroyWindow(app->window);
        SDL_Quit();

        DestroyTilemap(&app->map);

        free((void *)app->keyDownPrev);
        free(app);
    }

    return 0;
}
