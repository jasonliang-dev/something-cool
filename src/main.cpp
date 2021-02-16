#define SDL_MAIN_HANDLED

#ifdef _MSC_VER

// windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else

// linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#endif

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#include "language.h"
#include "app.h"

global const i32 SCREEN_WIDTH = 1366;
global const i32 SCREEN_HEIGHT = 768;

global const char *WINDOW_TITLE = "This is a title";

global AppState *app = NULL;

#include "maths.cpp"
#include "render.cpp"
#include "tilemap.cpp"
#include "entity.cpp"

internal inline bool KeyPress(SDL_Scancode scancode)
{
    return !app->keyDownPrev[scancode] && app->keyDown[scancode];
}

internal inline bool KeyRelease(SDL_Scancode scancode)
{
    return app->keyDownPrev[scancode] && !app->keyDown[scancode];
}

i32 main(i32 argc, char *argv[])
{
    (void)argc;
    (void)argv;

    {
        // init app
        app = static_cast<AppState *>(malloc(sizeof *app));
        assert(app);
        app->debug = false;
        app->running = true;
        app->scale = 4;
        app->camera = {0, 0};

        // init sdl
        assert(SDL_Init(SDL_INIT_VIDEO) == 0);

        app->window =
            SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        assert(app->window);

        app->renderer = SDL_CreateRenderer(app->window, -1,
                                           SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        assert(app->renderer);

        // init sdl_image
        i32 imgFlags = IMG_INIT_PNG;
        i32 imgInit = IMG_Init(imgFlags);
        assert((imgInit & imgFlags) == imgFlags);

        // init sdl_ttf
        assert(TTF_Init() != -1);

        // keyboard state
        app->keyDown = SDL_GetKeyboardState(&app->keyCount);
        app->keyDownPrev = static_cast<u8 *>(malloc(app->keyCount));
        assert(app->keyDownPrev);

        // load resources
        app->fntSans = TTF_OpenFont("data/SourceSansPro-Regular.ttf", 28);
        assert(app->fntSans);
        app->player.image = Texture_LoadFromImage("data/dog.png");
        Tilemap_Load(&app->map, "data/atlas.png", "data/test.json");

        // init entities
        Entity_ZeroMovement(&app->player);
    }

    SDL_Event e;

    u64 counterCurrent = SDL_GetPerformanceCounter();
    u64 counterNew;
    while (app->running)
    {
        counterNew = SDL_GetPerformanceCounter();
        app->deltaTime =
            (counterNew - counterCurrent) / static_cast<f32>(SDL_GetPerformanceFrequency());
        counterCurrent = counterNew;

        // app->keyDown updates after event loop.
        // must update keyDownPrev before the loop.
        memcpy(const_cast<u8 *>(app->keyDownPrev), app->keyDown, app->keyCount);

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                app->running = false;
            }
        }

        // update
        if (KeyPress(SDL_SCANCODE_F3))
        {
            app->debug = !app->debug;
        }

        if (KeyPress(SDL_SCANCODE_RETURN))
        {
            app->player.position = {0, 0};
        }

        f32 gravity = 40.0f * app->deltaTime;
        app->player.velocity.y += gravity;
        app->player.velocity.x -= app->player.velocity.x * app->deltaTime * 12.0f;

        f32 maxMove = 500.0f * app->deltaTime;
        f32 moveX = static_cast<f32>(app->keyDown[SDL_SCANCODE_D] - app->keyDown[SDL_SCANCODE_A]) *
                    80.0f * app->deltaTime;

        if (Signf(app->player.velocity.x + moveX) * (app->player.velocity.x + moveX) < maxMove)
        {
            app->player.velocity.x += moveX;
        }

        if (KeyPress(SDL_SCANCODE_SPACE))
        {
            app->player.velocity.y = -800.0f * app->deltaTime;
        }

        Entity_Move(&app->map, &app->player);

        v2 screenBox = {SCREEN_WIDTH, SCREEN_HEIGHT};
        v2 cameraOffset =
            screenBox - Texture_ToV2(&app->player.image) * static_cast<f32>(app->scale);
        cameraOffset /= 2;

        app->camera += (app->player.position - app->camera - cameraOffset) * app->deltaTime * 6.0f;
        app->camera = V2Clamp(app->camera, {0, 0}, Tilemap_BoundingBox(&app->map) - screenBox);

        // render
        SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(app->renderer);
        Tilemap_Draw(&app->map, 0, 0);
        Texture_DrawWorld(&app->player.image, static_cast<i32>(app->player.position.x),
                          static_cast<i32>(app->player.position.y));

        if (app->debug)
        {
            Font_RenderText(app->fntSans, 0, 0, "FPS: %d", static_cast<i32>(1.0f / app->deltaTime));
        }

        SDL_RenderPresent(app->renderer);
    }

    // free all resources
    {
        TTF_CloseFont(app->fntSans);
        Tilemap_Free(&app->map);
        SDL_DestroyTexture(app->player.image.texture);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);

        TTF_Quit();
        IMG_Quit();
        SDL_Quit();

        free(app);
    }

    return 0;
}
