#include "app.h"

APP_PERMANENT_LOAD
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    GL_LoadProcedures();

    tilemap *map = R_LoadTilemap("res/small.json", 0);

    char buff[64];
    for (u32 slow = 0; slow < map->height; slow++)
    {
        for (u32 fast = 0; fast < map->width; fast++)
        {
            sprintf(buff, "%d ", map->data[slow * map->height + fast]);
            OS_DebugPrint(buff);
        }
        OS_DebugPrint("\n");
    }
    OS_DebugPrint("\n");
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
    OS_GLSwapBuffers();
}

APP_CLOSE
{
    OS_DebugPrint("APP_CLOSE\n");
}
