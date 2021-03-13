#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4100)
#    pragma warning(disable : 4127)
#    pragma warning(disable : 4477)
#    pragma warning(disable : 4701)
#else
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
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

#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <glad.c>

#ifdef _MSC_VER
#    pragma warning(pop)
#else
#    pragma GCC diagnostic pop
#endif

#include "language.h"
#include "audio.h"
#include "render.h"
#include "shaders.gen.h"
#include "assets.h"
#include "app.h"

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768

#define PIXEL_ART_SCALE 4

#define WINDOW_TITLE "This is a title"

global AppState *app = NULL;
global Assets *assets = NULL;

#include "log.c"

#if defined(_WIN32)

#    include <windows.h>
#    include "windows_os_procs.c"

#    define HOT_LIB_PATH "hot.dll"
#    define HOT_COPY_LIB_PATH "hot_copy.dll"

#elif defined(__linux__)

#    include <fcntl.h>
#    include <unistd.h>
#    include <sys/stat.h>
#    include <sys/sendfile.h>
#    include "linux_os_procs.c"

#    define HOT_LIB_PATH "./hot.so"
#    define HOT_COPY_LIB_PATH "./hot_copy.so"

#else

#    error "platform not supported"

#endif

#include "memory.c"
#include "strings.c"
#include "maths.c"
#include "gl.c"
#include "audio.c"
#include "render.c"
#include "assets.c"
#include "hot_stub.c"
#include "hot_code_load.c"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    StartLog("app.log");

    app = MemAlloc(sizeof(AppState));
    assets = MemAlloc(sizeof(Assets));

    app->running = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
    {
        LogInfo("Initalized SDL");
    }
    else
    {
        LogFatal("Cannot initialize SDL");
    }

    app->windowWidth = SCREEN_WIDTH;
    app->windowHeight = SCREEN_HEIGHT;
    app->window = SDL_CreateWindow(
        WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (app->window)
    {
        LogInfo("Created window");
    }
    else
    {
        LogFatal("Cannot create window");
    }

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

    if (gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        LogInfo("Loaded OpenGL procedures");
        LogInfo("OpenGL %d.%d", GLVersion.major, GLVersion.minor);
    }
    else
    {
        LogFatal("Could not load OpenGL procedures");
    }

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GL_MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL,
                          GL_FALSE);
#endif

    InitAudio(&app->audio);
    LogInfo("Initalized audio");
    CreateRenderer(&app->renderer);
    LogInfo("Initalized renderer");
    CreateAllAssets();
    LogInfo("Initalized assets");

    app->keyDown = SDL_GetKeyboardState(&app->keyCount);
    app->keyDownPrev = MemAlloc(app->keyCount);

    HotCode hotCode;
    HotCodeLoad(&hotCode, HOT_LIB_PATH, HOT_COPY_LIB_PATH);
    LogInfo("Loaded hot code");

    v2 boyPos = v2(0, 0);
    v2 cameraPos = v2(0, 0);

    SDL_PauseAudioDevice(app->audio.device, false);

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
        }

        glViewport(0, 0, (int)app->windowWidth, (int)app->windowHeight);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        BeginDraw(&app->renderer, cameraPos);
        DrawTilemap(&app->renderer, &assets->map);
        DrawSpriteAnimation(&app->renderer, &assets->boy, boyPos);
        DrawTextSlow(&app->renderer, &assets->font, "!@#$%^&*() Hello World", v2(400, 300));
        FlushRenderer(&app->renderer);

        SDL_GL_SwapWindow(app->window);

        GL_CheckForErrors();

        HotCodeMaybeReload(&hotCode, HOT_LIB_PATH, HOT_COPY_LIB_PATH);
    }

    {
        DestroyRenderer(&app->renderer);

        SDL_GL_DeleteContext(app->glContext);
        SDL_DestroyWindow(app->window);
        SDL_Quit();

        DestroyAllAssets();
        free(assets);

        free((void *)app->keyDownPrev);
        free(app);

        EndLog();
    }

    return 0;
}
