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
    u32 shootCooldown;
    f32 moveSpeed;
    b32 facingRight;
    texture sprite;
};

typedef struct bone bone;
struct bone
{
    v2 pos;
    v2 vel;
    f32 rot;
};

typedef struct app_state app_state;
struct app_state
{
    b8 keysDown[Key_Max];
    b8 mouseDown[MouseButton_Max];

    u32 scale;
    u32 mapShader;
    u32 spriteShader;
    u32 vao;
    texture face;
    texture cursor;
    texture bone;
    tilemap map;

    player p;
    u32 bulletPoolCount;
    u32 bulletPoolMax;
    bone *bulletPool;
};

global app_state state;

#include "os.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"
#include "render.c"

internal v2 GetWorldCursorPosition(void)
{
    return v2(os->mousePosition.x / state.scale, os->mousePosition.y / state.scale);
}

void AppLoad(os_state *os_)
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    MemorySet(state.keysDown, 0, Key_Max);
    state.scale = 2;
    state.bulletPoolCount = 0;
    state.bulletPoolMax = 1000;
    state.bulletPool = M_ArenaPush(&os->permanentArena, sizeof(bone) * state.bulletPoolMax);

    state.p.moveSpeed = 3.0f;
    state.p.shootCooldown = 0;

    OS_ShowCursor(0);
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
    state.cursor = R_CreateTexture("res/cursor.png");
    state.bone = R_CreateTexture("res/bone.png");
    state.p.sprite = R_CreateTexture("res/dog.png");
    state.map = R_CreateTilemap("res/map.json", R_CreateTexture("res/atlas.png"), state.vao);

    GL_CheckForErrors();
}

void AppUpdate(void)
{
    player *p = &state.p;
    p->vel = v2(0.0f, 0.0f);

    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_WindowResize)
        {
            glViewport(0, 0, os->windowResolution.x, os->windowResolution.y);
            R_Update2DProjection(state.spriteShader, state.scale);
            R_Update2DProjection(state.mapShader, state.scale);
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
        else if (event.type == OS_EventType_MousePress)
        {
            state.mouseDown[event.mouseButton] = 1;
        }
        else if (event.type == OS_EventType_MouseRelease)
        {
            state.mouseDown[event.mouseButton] = 0;
        }
    }

    if (state.mouseDown[MouseButton_Left] && p->shootCooldown == 0)
    {
        p->shootCooldown = 10;
        bone *b = &state.bulletPool[state.bulletPoolCount++];
        v2 cursor = GetWorldCursorPosition();
        b->pos = p->pos;
        b->rot = PointDirection(p->pos, cursor);
        b->vel.x = 8.0f * Cos(b->rot);
        b->vel.y = 8.0f * Sin(b->rot);
    }

    for (u32 i = 0; i < state.bulletPoolCount; i++)
    {
        bone *b = &state.bulletPool[i];
        b->pos.x += b->vel.x;
        b->pos.y += b->vel.y;
    }

    if (p->shootCooldown > 0)
    {
        p->shootCooldown--;
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
    R_DrawSprite(state.spriteShader, state.vao, state.cursor,
                 v2(os->mousePosition.x / state.scale - (state.cursor.width / 2.0f),
                    os->mousePosition.y / state.scale - (state.cursor.height / 2.0f)),
                 0);

    for (u32 i = 0; i < state.bulletPoolCount; i++)
    {
        bone *b = &state.bulletPool[i];
        R_DrawSprite(state.spriteShader, state.vao, state.bone, b->pos, -b->rot);
    }

    OS_GLSwapBuffers();

    GL_CheckForErrors();
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
