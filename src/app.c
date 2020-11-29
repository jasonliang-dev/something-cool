#include "app.h"

u32 shader;
m4 projection;
u32 spriteVAO;
u32 spriteTexture;

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("APP_PERMANENT_LOAD\n");
    LoadOpenGLProcedures();

    shader = R_CreateShader(globalSpriteVertexShader, globalSpriteFragmentShader);
    glUseProgram(shader);

    projection =
        M4Ortho(0.0f, (f32)os->windowResolution.x, (f32)os->windowResolution.y, 0.0f, -1.0f, 1.0f);
    glUniform1i(glGetUniformLocation(shader, "image"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, 0,
                       (f32 *)projection.elements);

    spriteVAO = R_InitSpriteRenderer();
    spriteTexture = R_CreateTextureFromFile("res/awesomeface.png");
}

APP_HOT_LOAD
{
    os = os_;
    os->DebugPrint("APP_HOT_LOAD\n");
}

APP_HOT_UNLOAD
{
    os->DebugPrint("APP_HOT_UNLOAD\n");
}

APP_UPDATE
{
    os_event event;
    while (OS_GetNextEvent(&event))
    {
    }

    glClear(GL_COLOR_BUFFER_BIT);
    R_DrawSprite(spriteVAO, shader, spriteTexture, v2(200.0f, 200.0f),
                 v2(300.0f, 400.0f), 45.0f, v3(0.0f, 1.0f, 0.0f));
    os->SwapBuffers();
}

APP_CLOSE
{
    os->DebugPrint("APP_CLOSE\n");
}
