#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4477)
#pragma warning(disable : 4701)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>

#include <glad/glad.h>
#undef APIENTRY // man, clean up after yourself.

#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include <nuklear.h>
#define NK_SDL_GL3_IMPLEMENTATION
#include <nuklear_sdl_gl3.h>

#include <glad.c>
#include <nuklear_overview.c>

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include "language.h"
#include "audio.h"
#include "render.h"
#include "shaders.gen.h"
#include "app.h"

#define NK_MAX_VERTEX_MEMORY (512 * 1024)
#define NK_MAX_ELEMENT_MEMORY (128 * 1024)

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768

#define PIXEL_ART_SCALE 4

#define WINDOW_TITLE "This is a title"

global AppState *app = NULL;

#include "utils.c"
#include "maths.c"
#include "gl.c"
#include "audio.c"
#include "render.c"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    app = malloc(sizeof(AppState));
    assert(app);
    app->running = true;
    app->showOverview = false;

    assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0);

    app->windowWidth = SCREEN_WIDTH;
    app->windowHeight = SCREEN_HEIGHT;
    app->window = SDL_CreateWindow(
        WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    assert(app->window);

    i32 glContextFlags = 0;
#ifdef __APPLE__
    // always required on Mac
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

    app->glContext = SDL_GL_CreateContext(app->window);
    SDL_GL_MakeCurrent(app->window, app->glContext);
    SDL_GL_SetSwapInterval(0); // vsync

    assert(gladLoadGLLoader(SDL_GL_GetProcAddress));

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GL_MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL,
                          GL_FALSE);
#endif

    InitAudio(&app->audio);
    CreateRenderer(&app->renderer);

    {
        app->dog = CreateTexture("data/dog.png");
        CreateSpriteAnimation(&app->boy, "data/boy-idle.png", 32, 50);
        CreateTilemap(&app->map, "data/tiles.png", "data/map.json");
        CreateSoundFromWAV(&app->wobble, "data/wobble.wav");
    }

    app->keyDown = SDL_GetKeyboardState(&app->keyCount);
    app->keyDownPrev = malloc(app->keyCount);
    assert(app->keyDownPrev);

    app->nkContext = nk_sdl_init(app->window);
    {
        struct nk_font_atlas *atlas;
        nk_sdl_font_stash_begin(&atlas);
        nk_sdl_font_stash_end();
    }

    v2 boyPos = v2(0, 0);
    v2 cameraPos = v2(0, 0);
    v2 dogPosition = v2(100, 100);
    f32 rotation = 0.0f;
    struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.10f, bg.b = 0.10f, bg.a = 1.0f;

    SDL_PauseAudioDevice(app->audio.device, false);
    PlaySound(&app->audio, &app->wobble);

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

        if (nk_begin(app->nkContext, "Window", nk_rect(50, 50, 300, 300),
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                         NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            struct nk_context *ctx = app->nkContext;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "show overview", &app->showOverview);

            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_button_label(ctx, "Play Sound"))
            {
                PlaySound(&app->audio, &app->wobble);
            }

            nk_layout_row_dynamic(ctx, 25, 2);
            nk_property_float(ctx, "camera x:", -F32_MAX, &cameraPos.x, F32_MAX, 1.0f, 3.0f);
            nk_property_float(ctx, "camera y:", -F32_MAX, &cameraPos.y, F32_MAX, 1.0f, 3.0f);

            nk_layout_row_dynamic(ctx, 25, 2);
            nk_property_float(ctx, "boy x:", -F32_MAX, &boyPos.x, F32_MAX, 1.0f, 1.0f);
            nk_property_float(ctx, "boy y:", -F32_MAX, &boyPos.y, F32_MAX, 1.0f, 1.0f);

            nk_layout_row_dynamic(ctx, 25, 2);
            nk_property_float(ctx, "dog x:", -F32_MAX, &dogPosition.x, F32_MAX, 1.0f, 1.0f);
            nk_property_float(ctx, "dog y:", -F32_MAX, &dogPosition.y, F32_MAX, 1.0f, 1.0f);

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_float(ctx, "player rot:", -F32_MAX, &rotation, F32_MAX, 0.1f, 0.01f);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);

            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400)))
            {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_TEXT_LEFT, "deltaTime: %.04f (%.02f FPS)", app->deltaTime * 1000.0f,
                      1 / app->deltaTime);
        }
        nk_end(app->nkContext);

        if (app->showOverview)
        {
            nk_overview(app->nkContext);
        }

        glViewport(0, 0, (int)app->windowWidth, (int)app->windowHeight);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        glClear(GL_COLOR_BUFFER_BIT);

        BeginDraw(&app->renderer, cameraPos);
        DrawTilemap(&app->renderer, &app->map);
        DrawTexture(&app->renderer, app->dog, dogPosition, rotation);
        DrawSpriteAnimation(&app->renderer, &app->boy, boyPos);
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
