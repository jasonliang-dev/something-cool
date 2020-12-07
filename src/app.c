#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"

#define CUTE_TILED_IMPLEMENTATION
#include "ext/cute_tiled.h"

#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "os.h"
#include "opengl.h"
#include "render.h"
#include "scene.h"
#include "app.h"

global app_state_t app;
global os_state_t *os = NULL;

#include "os.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"
#include "render.c"
#include "scene_game.c"

void AppLoad(os_state_t *os_)
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    app.sceneArena = M_ArenaInitialize(Gigabytes(4));
    app.scratchArena = M_ArenaInitialize(Gigabytes(4));

    scene_t gameScene;
    gameScene.Init = GameSceneInit;
    gameScene.Destroy = GameSceneDestroy;
    gameScene.Update = GameSceneUpdate;

    app.scene = gameScene;

    MemorySet(app.keysDown, 0, Key_Max);
    MemorySet(app.mouseDown, 0, MouseButton_Max);
    app.scale = 4;

    OS_ShowCursor(0);
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

    app.resources.face = R_CreateTexture("res/awesomeface.png");
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
            if (event.key == Key_F11)
            {
                os->fullscreen = !os->fullscreen;
            }
            break;
        case OS_EventType_KeyDown:
            app.keysDown[event.key] = 1;
            break;
        case OS_EventType_KeyUp:
            app.keysDown[event.key] = 0;
            break;
        case OS_EventType_MouseDown:
            app.mouseDown[event.mouseButton] = 1;
            break;
        case OS_EventType_MouseUp:
            app.mouseDown[event.mouseButton] = 0;
            break;
        }
    }

    app.scene.Update(&app.sceneArena);

    GL_CheckForErrors();
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
