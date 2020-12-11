#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define CUTE_TILED_IMPLEMENTATION
#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ext/stb_truetype.h"
#include "ext/stb_image.h"
#include "ext/cute_tiled.h"

#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "os.h"
#include "opengl.h"
#include "render.h"
#include "scene.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "shaders.h"
#include "ui.h"
#include "app.h"

global app_state_t *app;
global os_state_t *os = NULL;

#include "app_utils.c"
#include "os.c"
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

    // ShowCursor(0);

    os->permanentArena = M_ArenaInitialize(Gigabytes(1));
    app = M_ArenaPushZero(&os->permanentArena, sizeof(app_state_t));
    app->sceneArena = M_ArenaInitialize(Gigabytes(4));
    app->scratchArena = M_ArenaInitialize(Gigabytes(4));

    app->scene = SceneCreate(Menu);

    MemorySet(app->keyDown, 0, Key_Max);
    MemorySet(app->mouseDown, 0, MouseButton_Max);
    app->screenScale = v2(LOW_RES_SCREEN_WIDTH / (f32)os->windowResolution.x,
                          LOW_RES_SCREEN_HEIGHT / (f32)os->windowResolution.y);

    R_SetupRendering();

    i32 params;
    glBindFramebuffer(GL_FRAMEBUFFER, app->screenFBO);
    glGetIntegerv(GL_SAMPLE_BUFFERS, &params);
    OS_DebugPrint("GL_SAMPLE_BUFFERS: %d\n", params);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGetIntegerv(GL_SAMPLE_BUFFERS, &params);
    OS_DebugPrint("GL_SAMPLE_BUFFERS: %d\n", params);

    app->resources.play = R_CreateTexture("res/play.png");
    app->resources.quit = R_CreateTexture("res/quit.png");
    app->resources.cursor = R_CreateTexture("res/cursor.png");
    app->resources.bone = R_CreateTexture("res/bone.png");
    app->resources.dog = R_CreateTexture("res/dog.png");
    app->resources.atlas = R_CreateTexture("res/atlas.png");
    app->resources.map = R_CreateTilemap("res/map.json", app->resources.atlas);

    app->scene.Begin(&app->sceneArena);

    GL_CheckForErrors();
}

void AppUpdate(void)
{
    MemorySet(app->keyPress, 0, Key_Max);
    MemorySet(app->mousePress, 0, MouseButton_Max);

    os_event_t event;
    while (OS_GetNextEvent(&event))
    {
        switch (event.type)
        {
        case OS_EventType_WindowResize:
            app->screenScale = v2(LOW_RES_SCREEN_WIDTH / (f32)os->windowResolution.x,
                                  LOW_RES_SCREEN_HEIGHT / (f32)os->windowResolution.y);
            break;
        case OS_EventType_KeyPress:
            app->keyPress[event.key] = 1;
            break;
        case OS_EventType_KeyDown:
            app->keyDown[event.key] = 1;
            break;
        case OS_EventType_KeyUp:
            app->keyDown[event.key] = 0;
            break;
        case OS_EventType_MouseDown:
            app->mousePress[event.mouseButton] = 1;
            app->mouseDown[event.mouseButton] = 1;
            break;
        case OS_EventType_MouseUp:
            app->mouseDown[event.mouseButton] = 0;
            break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, app->screenFBO);
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
    glBlitFramebuffer(0, 0, LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT, 0, 0,
                      os->windowResolution.x, os->windowResolution.y, GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);

    OS_GLSwapBuffers();

    GL_CheckForErrors();

    if (app->keyPress[Key_F11])
    {
        os->fullscreen = !os->fullscreen;
    }
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
