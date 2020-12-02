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
    u32 spriteShader;
    u32 vao;
    texture face;
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

    GL_LoadProcedures();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.spriteShader = R_InitShader(quadVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(state.spriteShader);
    R_UpdateSpriteProjection(state.spriteShader);
    glUniform1i(glGetUniformLocation(state.spriteShader, "image"), 0);
    state.vao = R_CreateQuadVAO();
    state.face = R_CreateTexture("res/awesomeface.png");
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
    OS_GLSwapBuffers();
}

void AppClose(void)
{
    OS_DebugPrint("APP_CLOSE\n");
}
