#include "app.h"
#include <stdio.h>

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("PERMANENT_LOAD\n");
}

APP_HOT_LOAD
{
    os = os_;
    os->DebugPrint("HOT_LOAD\n");
}

APP_HOT_UNLOAD
{
    os->DebugPrint("HOT_UNLOAD\n");
}

APP_UPDATE
{
    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_MousePress)
        {
            os->DebugPrint("Mouse clicked!\n");
        }
        else if (event.type == OS_EventType_MouseMove)
        {
            TCHAR buff[256];

            sprintf_s(buff, sizeof(buff), "x = %.02f, y = %.02f\n", os->mousePosition.x,
                      os->mousePosition.y);
            os->DebugPrint(buff);
        }
    }

    local_persist f32 tick = 0;

    f32 bri = Sin(tick++ * 0.1f) + 0.5f;
    glClearColor(bri, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    os->GLSwapBuffers();
}
