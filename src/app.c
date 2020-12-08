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
#include "app.h"

global app_state_t app;
global os_state_t *os = NULL;

#include "os.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"
#include "render.c"
#include "scene_game.c"
#include "scene_menu.c"

void AppLoad(os_state_t *os_)
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    ShowCursor(0);

    app.sceneArena = M_ArenaInitialize(Gigabytes(4));
    app.scratchArena = M_ArenaInitialize(Gigabytes(4));

    app.scene = Scene_CreateFrom(Game);

    MemorySet(app.keyDown, 0, Key_Max);
    MemorySet(app.mouseDown, 0, MouseButton_Max);
    app.scale = 4;

    GL_LoadProcedures();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    app.vao = R_CreateQuadVAO();

    app.spriteShader = R_InitShader(quadVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(app.spriteShader);
    glUniform1i(glGetUniformLocation(app.spriteShader, "image"), 0);

    app.mapShader = R_InitShader(tilemapVertexShaderSource, tilemapFragmentShaderSource);
    glUseProgram(app.mapShader);
    glUniform1i(glGetUniformLocation(app.mapShader, "atlas"), 1);

    app.resources.cursor = R_CreateTexture("res/cursor.png");
    app.resources.bone = R_CreateTexture("res/bone.png");
    app.resources.dog = R_CreateTexture("res/dog.png");
    app.resources.atlas = R_CreateTexture("res/atlas.png");
    app.resources.map = R_CreateTilemap("res/map.json", app.resources.atlas, app.vao);

    app.scene.Init(&app.sceneArena);

    GL_CheckForErrors();
}

void AppUpdate(void)
{
    MemorySet(app.keyPress, 0, Key_Max);

    os_event_t event;
    while (OS_GetNextEvent(&event))
    {
        switch (event.type)
        {
        case OS_EventType_WindowResize:
            glViewport(0, 0, os->windowResolution.x, os->windowResolution.y);
            R_Update2DProjection(app.spriteShader, app.scale);
            R_Update2DProjection(app.mapShader, app.scale);
            break;
        case OS_EventType_KeyPress:
            app.keyPress[event.key] = 1;
            break;
        case OS_EventType_KeyDown:
            app.keyDown[event.key] = 1;
            break;
        case OS_EventType_KeyUp:
            app.keyDown[event.key] = 0;
            break;
        case OS_EventType_MouseDown:
            app.mouseDown[event.mouseButton] = 1;
            break;
        case OS_EventType_MouseUp:
            app.mouseDown[event.mouseButton] = 0;
            break;
        }
    }

    scene_t nextScene;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (app.scene.Update(app.sceneArena.base, &nextScene))
    {
        app.scene.Destroy(app.sceneArena.base);
        M_ArenaClear(&app.sceneArena);
        app.scene = nextScene;
        app.scene.Init(&app.sceneArena);
    }
    OS_GLSwapBuffers();

    GL_CheckForErrors();

    if (app.keyPress[Key_F11])
    {
        os->fullscreen = !os->fullscreen;
    }
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
