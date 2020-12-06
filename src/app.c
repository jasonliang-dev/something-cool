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

typedef struct player player;
struct player
{
    v2 pos;
    v2 vel;
    f32 moveSpeed;
    b32 facingRight;
    texture sprite;
};

typedef struct app_state app_state;
struct app_state
{
    b8 keysDown[Key_Max];

    u32 mapShader;
    u32 spriteShader;
    u32 vao;
    texture face;
    tilemap map;

    player p;

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

    MemorySet(state.keysDown, 0, Key_Max);
    state.permanentArena = M_ArenaInitialize(Gigabytes(4));

    state.p.moveSpeed = 3.0f;

    GL_LoadProcedures();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.vao = R_CreateQuadVAO();

    state.spriteShader = R_InitShader(quadVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(state.spriteShader);
    glUniform1i(glGetUniformLocation(state.spriteShader, "image"), 0);

    state.mapShader = R_InitShader(tilemapVertexShaderSource, tilemapFragmentShaderSource);
    glUseProgram(state.mapShader);
    glUniform1i(glGetUniformLocation(state.mapShader, "atlas"), 1);

    state.face = R_CreateTexture("res/awesomeface.png");
    state.p.sprite = R_CreateTexture("res/dog.png");
    state.map = R_CreateTilemap("res/map.json", R_CreateTexture("res/atlas.png"), state.vao);

    GL_CheckForErrors();
}

void AppUpdate(void)
{
    local_persist f32 angle = 0;

    player *p = &state.p;
    p->vel = v2(0.0f, 0.0f);

    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_WindowResize)
        {
            glViewport(0, 0, os->windowResolution.x, os->windowResolution.y);
            R_Update2DProjection(state.spriteShader);
            R_Update2DProjection(state.mapShader);
        }
        else if (event.type == OS_EventType_KeyPress)
        {
            if (event.key == Key_F11)
            {
                os->fullscreen = !os->fullscreen;
            }

            state.keysDown[event.key] = 1;
        }
        else if (event.type == OS_EventType_KeyRelease)
        {
            state.keysDown[event.key] = 0;
        }
    }

    if (state.keysDown[Key_W])
    {
        p->vel.y -= 1.0f;
    }
    if (state.keysDown[Key_S])
    {
        p->vel.y += 1.0f;
    }
    if (state.keysDown[Key_A])
    {
        p->vel.x -= 1.0f;
    }
    if (state.keysDown[Key_D])
    {
        p->vel.x += 1.0f;
    }

    p->vel = V2Normalize(p->vel);
    p->vel = V2MultiplyF32(p->vel, p->moveSpeed);

    p->pos.x += p->vel.x;
    p->pos.y += p->vel.y;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    R_DrawTilemap(state.mapShader, state.vao, state.map);
    R_DrawSprite(state.spriteShader, state.vao, state.p.sprite, state.p.pos, 0);

    OS_GLSwapBuffers();

    GL_CheckForErrors();
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
