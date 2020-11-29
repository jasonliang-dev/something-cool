#include "app.h"

sprite_data sprite;

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("APP_PERMANENT_LOAD\n");

    GL_LoadProcedures();
    R_InitializeShaders();

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
    os_event event;
    while (OS_GetNextEvent(&event))
    {
    }

    glClear(GL_COLOR_BUFFER_BIT);
    R_DrawSprite(sprite, v2(0, 0), 0);
    os->SwapBuffers();
}

APP_CLOSE
{
    os->DebugPrint("APP_CLOSE\n");
}
