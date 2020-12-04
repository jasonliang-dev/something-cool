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

typedef struct app_state app_state;
struct app_state
{
    u32 mapShader;
    u32 spriteShader;
    u32 vao;
    texture face;
    texture atlas;
    texture test;
    tilemap map;

    memory_arena permanentArena;
};

global app_state state;

#include "os.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"
#include "render.c"

void AppLoad(os_state *os_)
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    state.permanentArena = M_ArenaInitialize(Gigabytes(4));

    GL_LoadProcedures();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.vao = R_CreateQuadVAO();

    /*
    state.spriteShader = R_InitShader(quadVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(state.spriteShader);
    R_Update2DProjection(state.spriteShader);
    glUniform1i(glGetUniformLocation(state.spriteShader, "image"), 1);

    state.face = R_CreateTexture("res/awesomeface.png");
    */

    state.mapShader = R_InitShader(quadVertexShaderSource, tilemapFragmentShaderSource);
    glUseProgram(state.mapShader);
    R_Update2DProjection(state.mapShader);
    glUniform1i(glGetUniformLocation(state.mapShader, "MapTexture"), 1);
    glUniform1i(glGetUniformLocation(state.mapShader, "AtlasTexture"), 2);

    state.atlas = R_CreateTexture("res/atlas.png");
    state.map = R_CreateTilemap("res/small.json", state.atlas);
    GL_CheckForErrors();
}

void AppUpdate(void)
{
    local_persist f32 angle = 0;

    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_WindowResize)
        {
            glViewport(0, 0, os->windowResolution.x, os->windowResolution.y);
            // R_Update2DProjection(state.spriteShader);
            R_Update2DProjection(state.mapShader);
        }
        else if (event.type == OS_EventType_KeyPress)
        {
            if (event.key == Key_F11)
            {
                os->fullscreen = !os->fullscreen;
            }
        }
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    /*
    R_DrawSprite(state.spriteShader, state.vao, state.face,
                 v2(os->windowResolution.x / 2.0f - state.face.width / 2.0f,
                    os->windowResolution.y / 2.0f - state.face.height / 2.0f),
                 angle += 0.01f);
    */
    // R_DrawSprite(state.spriteShader, state.vao, state.face, v2(0.0f, 0.0f), PI / 2.0f);
    // R_DrawSprite(state.spriteShader, state.vao, state.atlas, v2(0.0f, 0.0f), 0);
    R_DrawTilemap(state.mapShader, state.vao, state.map);
    OS_GLSwapBuffers();
    GL_CheckForErrors();
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
