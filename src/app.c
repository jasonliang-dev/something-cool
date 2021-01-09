#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CUTE_TILED_IMPLEMENTATION
#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

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
#include "shaders.h"
#include "app.h"

global app_state_t *app;
global os_state_t *os = NULL;

#include "app_utils.c"
#include "os.c"
#include "audio.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"
#include "render.c"
#include "ui.c"
#include "scene_game.c"
#include "scene_menu.c"

void AppLoad(os_state_t *os_)
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    os->permanentArena = M_ArenaInitialize(Gigabytes(1));
    app = M_ArenaPushZero(&os->permanentArena, sizeof(app_state_t));
    app->sceneArena = M_ArenaInitialize(Gigabytes(4));
    app->scratchArena = M_ArenaInitialize(Gigabytes(4));

    MemorySet(app->keyDown, 0, sizeof(app->keyDown));
    MemorySet(app->mouseDown, 0, sizeof(app->mouseDown));
    app->renderer.screenScale = v2(LOW_RES_SCREEN_WIDTH / (f32)os->windowWidth,
                                   LOW_RES_SCREEN_HEIGHT / (f32)os->windowHeight);

    {
        R_SetupRendering(&app->renderer);

        app_resources_t *resources = &app->resources;
        resources->sndJingle = Sound_LoadFromFile("res/jingle.ogg");
        resources->sndImpact = Sound_LoadFromFile("res/impact.ogg");
        resources->texPlay = R_CreateTexture("res/play.png");
        resources->texQuit = R_CreateTexture("res/quit.png");
        resources->texCursor = R_CreateTexture("res/cursor.png");
        resources->texBone = R_CreateTexture("res/bone.png");
        resources->texDog = R_CreateTexture("res/dog.png");
        resources->texAtlas = R_CreateTexture("res/atlas.png");
        resources->map = R_CreateTilemap("res/map.json", resources->texAtlas);

        app->scene = SceneCreate(Menu);
        app->scene.Begin(&app->sceneArena);

        GL_CheckForErrors();
    }

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

    glBindFramebuffer(GL_FRAMEBUFFER, app->renderer.screenFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    scene_t nextScene;
    if (app->scene.Update(app->sceneArena.base, &nextScene))
    {
        app->scene.End(app->sceneArena.base);
        M_ArenaClear(&app->sceneArena);
        app->scene = nextScene;
        app->scene.Begin(&app->sceneArena);
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT, 0, 0, os->windowWidth,
                      os->windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    OS_GLSwapBuffers();
    GL_CheckForErrors();

    if (app->keyPress[Key_F11])
    {
        os->fullscreen = !os->fullscreen;
    }

    Audio_Update(&app->audio);
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
