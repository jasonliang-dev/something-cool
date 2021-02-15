#define SDL_MAIN_HANDLED

#ifdef _MSC_VER

// windows
#include <SDL.h>
#include <SDL_image.h>

#else

// linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#endif

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#include "language.h"
#include "app.h"

global const i32 SCREEN_WIDTH = 1366;
global const i32 SCREEN_HEIGHT = 768;

global const char *WINDOW_TITLE = "This is a title";

global AppState *app = NULL;

#include "render.cpp"

internal inline v2 V2Normalize(v2 v)
{
    v2 result;
    f32 length = static_cast<f32>(sqrt(v.x * v.x + v.y * v.y));

    // normalizing the 0 vector???
    if (length <= FLT_EPSILON)
    {
        result.x = 0.0f;
        result.y = 0.0f;
    }
    else
    {
        result.x = v.x / length;
        result.y = v.y / length;
    }

    return result;
}

internal inline bool KeyPress(SDL_Scancode scancode)
{
    return !app->keyDownPrev[scancode] && app->keyDown[scancode];
}

internal inline bool KeyRelease(SDL_Scancode scancode)
{
    return app->keyDownPrev[scancode] && !app->keyDown[scancode];
}

i32 main(void)
{
    {
        // init app
        app = static_cast<AppState *>(malloc(sizeof *app));
        assert(app);
        app->running = true;
        app->scale = 5;
        app->camera = {0, 0};

        // init sdl
        assert(SDL_Init(SDL_INIT_VIDEO) == 0);

        app->window =
            SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        assert(app->window);

        app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
        assert(app->renderer);

        SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0x00);

        // init sdl_image
        i32 imgFlags = IMG_INIT_PNG;
        i32 imgInit = IMG_Init(imgFlags);
        assert((imgInit & imgFlags) == imgFlags);

        // keyboard state
        app->keyDown = SDL_GetKeyboardState(&app->keyCount);
        app->keyDownPrev = static_cast<u8 *>(malloc(app->keyCount));
        assert(app->keyDownPrev);

        // load resources
        app->texDog = Texture_Load("data/dog.png");
        Tilemap_Load(&app->map, "data/atlas.png", "data/test.json");
    }

    SDL_Event e;
    v2 player = {0, 0};

    u64 counterCurrent = SDL_GetPerformanceCounter();
    u64 counterNew;
    while (app->running)
    {
        counterNew = SDL_GetPerformanceCounter();
        app->deltaTime =
            (counterNew - counterCurrent) / static_cast<f32>(SDL_GetPerformanceFrequency());
        counterCurrent = counterNew;

        printf("delta: %f, FPS: %f\n", app->deltaTime, 1.0f / app->deltaTime);

        // app->keyDown updates after event loop.
        // must update keyDownPrev before the loop.
        memcpy(app->keyDownPrev, app->keyDown, app->keyCount);
        // event
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                app->running = false;
            }
        }

        // update
        if (KeyPress(SDL_SCANCODE_SPACE) || KeyRelease(SDL_SCANCODE_RETURN))
        {
            player = {0, 0};
        }

        v2 playerVel = {0, 0};

        if (app->keyDown[SDL_SCANCODE_W])
        {
            playerVel.y -= 1;
        }
        if (app->keyDown[SDL_SCANCODE_A])
        {
            playerVel.x -= 1;
        }
        if (app->keyDown[SDL_SCANCODE_S])
        {
            playerVel.y += 1;
        }
        if (app->keyDown[SDL_SCANCODE_D])
        {
            playerVel.x += 1;
        }

        playerVel = V2Normalize(playerVel);
        player.x += playerVel.x * 400 * app->deltaTime;
        player.y += playerVel.y * 400 * app->deltaTime;

        app->camera.x +=
            (player.x - app->camera.x - ((SCREEN_WIDTH - (app->texDog.width * app->scale)) / 2)) *
            app->deltaTime * 6.0f;
        app->camera.y +=
            (player.y - app->camera.y - ((SCREEN_HEIGHT - (app->texDog.height * app->scale)) / 2)) *
            app->deltaTime * 6.0f;

        // render
        SDL_RenderClear(app->renderer);
        Tilemap_Draw(&app->map, 0, 0);
        Texture_Draw(&app->texDog, static_cast<i32>(player.x), static_cast<i32>(player.y));
        SDL_RenderPresent(app->renderer);
    }

    // free all resources
    {
        SDL_DestroyTexture(app->texDog.texture);
        SDL_DestroyTexture(app->map.atlas.texture);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);

        IMG_Quit();
        SDL_Quit();

        free(app);
    }

    return 0;
}
