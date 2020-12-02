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

    {
        i32 iwidth;
        i32 iheight;
        i32 channels;
        u32 *imageData = (u32 *)stbi_load("res/test.png", &iwidth, &iheight, &channels, 0);

        char buff[256];
        for (i32 slow = 0; slow < iheight; slow++)
        {
            for (i32 fast = 0; fast < iwidth; fast++)
            {
                u8 *pixel = (u8 *)&imageData[slow * iwidth + fast];
                sprintf(buff, "%d %d %d %d, ", pixel[0], pixel[1], pixel[2], pixel[3]);
                OS_DebugPrint(buff);
            }
            OS_DebugPrint("\n");
        }
        OS_DebugPrint("\n");

        stbi_image_free(imageData);
    }

    GL_LoadProcedures();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.spriteShader = R_InitShader(quadVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(state.spriteShader);
    R_UpdateSpriteProjection(state.spriteShader);
    glUniform1i(glGetUniformLocation(state.spriteShader, "image"), 0);

    state.vao = R_CreateQuadVAO();
    state.face = R_CreateTexture("res/awesomeface.png");

    state.mapShader = R_InitShader(quadVertexShaderSource, tilemapFragmentShaderSource);
    glUseProgram(state.mapShader);
    R_UpdateSpriteProjection(state.mapShader);
    glUniform1i(glGetUniformLocation(state.spriteShader, "MapTexture"), 1);
    glUniform1i(glGetUniformLocation(state.spriteShader, "TileAtlasTexture"), 2);

    state.map = R_CreateTilemap("res/small.json", R_CreateTexture("res/atlas.png"));
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
            R_UpdateSpriteProjection(state.spriteShader);
        }
        else if (event.type == OS_EventType_KeyPress)
        {
            if (event.key == Key_F11)
            {
                os->fullscreen = !os->fullscreen;
            }
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    R_DrawSprite(state.spriteShader, state.vao, state.face,
                 v2(os->windowResolution.x / 2.0f - state.face.width / 2.0f,
                    os->windowResolution.y / 2.0f - state.face.height / 2.0f),
                 angle += 0.01f);
    R_DrawTilemap(state.mapShader, state.vao, state.map);
    OS_GLSwapBuffers();
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
