#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CUTE_TILED_IMPLEMENTATION
#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#define STBI_ONLY_PNG

#if PLATFORM_WIN32

#include "ext/cute_tiled.h"
#include "ext/stb_image.h"
#include "ext/stb_truetype.h"

#pragma warning(push, 0)
#pragma warning(disable : 4701)
#include "ext/stb_vorbis.c"
#pragma warning(pop)

#elif PLATFORM_LINUX

#include "ext/cute_tiled.h"
#include "ext/stb_truetype.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include "ext/stb_image.h"
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ext/stb_vorbis.c"
#pragma GCC diagnostic pop

#else

#error "Platform not supported."

#endif

#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "os.h"
#include "audio.h"
#include "opengl.h"
#include "render.h"
#include "ui.h"
#include "scene.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "app.h"

global app_state_t *app;
global os_state_t *os = NULL;

#include "maths.cpp"
#include "app_utils.cpp"
#include "os.cpp"
#include "audio.cpp"
#include "memory.cpp"
#include "opengl.cpp"
#include "render.cpp"
#include "ui.cpp"
#include "scene_game.cpp"
#include "scene_menu.cpp"

void AppLoad(os_state_t *os_)
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    app = (app_state_t *)OS_Reserve(sizeof(app_state_t));
    OS_Commit(app, sizeof(app_state_t));

    app->permanentArena = Memory_ArenaInitialize(Gigabytes(1));
    app->sceneArena = Memory_ArenaInitialize(Gigabytes(4));
    app->scratchArena = Memory_ArenaInitialize(Gigabytes(1));

    MemorySet(app->keyDown, 0, sizeof(app->keyDown));
    MemorySet(app->mouseDown, 0, sizeof(app->mouseDown));
    app->renderer.screenScale = v2(LOW_RES_SCREEN_WIDTH / (f32)os->windowWidth,
                                   LOW_RES_SCREEN_HEIGHT / (f32)os->windowHeight);

    app->isWireframe = false;

    {
        Render_SetupRendering(&app->renderer);

        app_resources_t *resources = &app->resources;

        Audio_LoadSoundFromFile("res/jingle.ogg", &resources->sndJingle);
        Audio_LoadSoundFromFile("res/impact.ogg", &resources->sndImpact);

        Render_CreateFont("res/source-sans-pro.ttf", &resources->fntFont, 32.0f);

        Render_CreateTexture("res/play.png", &resources->texPlay);
        Render_CreateTexture("res/quit.png", &resources->texQuit);
        Render_CreateTexture("res/cursor.png", &resources->texCursor);
        Render_CreateTexture("res/bone.png", &resources->texBone);
        Render_CreateTexture("res/dog.png", &resources->texDog);
        Render_CreateTexture("res/atlas.png", &resources->texAtlas);

        Render_CreateTilemap("res/map.json", resources->texAtlas, &resources->map);

        Memory_ArenaClear(&app->scratchArena);
        GL_CheckForErrors();
    }

    app->scene = SceneCreate(Menu);
    app->scene.Begin(&app->sceneArena);

    UI_Init(&app->ui);

    Audio_Init(&app->audio);
    app->audio.volume = 0.2f;
}

void AppUpdate(void)
{
    MemorySet(app->keyPress, 0, sizeof(app->keyPress));
    MemorySet(app->mousePress, 0, sizeof(app->mousePress));

    os_event_t event;
    while (OS_GetNextEvent(&event))
    {
        switch (event.type)
        {
        case OS_EventType_WindowResize:
            app->renderer.screenScale = v2(LOW_RES_SCREEN_WIDTH / (f32)os->windowWidth,
                                           LOW_RES_SCREEN_HEIGHT / (f32)os->windowHeight);
            glUseProgram(app->renderer.shaders.font);
            Render_OrthoProjection(app->renderer.shaders.font,
                                   v2((f32)os->windowWidth, (f32)os->windowHeight));
            break;
        case OS_EventType_KeyPress:
            app->keyPress[event.key] = true;
            break;
        case OS_EventType_KeyDown:
            app->keyDown[event.key] = true;
            break;
        case OS_EventType_KeyUp:
            app->keyDown[event.key] = false;
            break;
        case OS_EventType_MouseDown:
            app->mousePress[event.mouseButton] = true;
            app->mouseDown[event.mouseButton] = true;
            break;
        case OS_EventType_MouseUp:
            app->mouseDown[event.mouseButton] = false;
            break;
        default:
            break;
        }
    }

    if (app->keyPress[OS_Key_F3])
    {
        glPolygonMode(GL_FRONT_AND_BACK,
                      (app->isWireframe = !app->isWireframe) ? GL_LINE : GL_FILL);
    }

    glViewport(0, 0, LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, app->renderer.pixelFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    b32 hasNextScene;
    scene_t nextScene;

    hasNextScene = app->scene.PixelUpdate(app->sceneArena.base, &nextScene);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT, 0, 0, os->windowWidth,
                      os->windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glViewport(0, 0, os->windowWidth, os->windowHeight);
    hasNextScene = app->scene.NativeUpdate(app->sceneArena.base, &nextScene) || hasNextScene;

    OS_GLSwapBuffers();
    GL_CheckForErrors();

    Audio_Update(&app->audio);

    if (hasNextScene)
    {
        app->scene.End(app->sceneArena.base);
        Memory_ArenaClear(&app->sceneArena);
        app->scene = nextScene;
        app->scene.Begin(&app->sceneArena);
    }

    if (app->keyPress[OS_Key_F11])
    {
        os->fullscreen = !os->fullscreen;
    }
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
