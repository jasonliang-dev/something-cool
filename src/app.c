#include "app.h"

global sprite_data sprite;

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("APP_PERMANENT_LOAD\n");

    GL_LoadProcedures();
    R_InitSpriteShader();

    sprite = R_CreateSprite("res/awesomeface.png");
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
    local_persist f32 angle = 0;

    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_WindowResize)
        {
            glViewport(0, 0, os->windowResolution.x, os->windowResolution.y);
            R_UpdateSpriteProjection();
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    R_DrawSprite(sprite, v2(0, 0), 0);
    R_DrawSprite(sprite,
                 v2(((f32)os->windowResolution.x / 2.0f) - ((f32)sprite.size.width / 2.0f),
                    ((f32)os->windowResolution.y / 2.0f) - ((f32)sprite.size.height / 2.0f)),
                 angle += 0.05f);
    os->SwapBuffers();
}

APP_CLOSE
{
    os->DebugPrint("APP_CLOSE\n");
}
