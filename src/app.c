#include "app.h"

APP_PERMANENT_LOAD
{
    os = os_;
    OS_DebugPrint("APP_PERMANENT_LOAD\n");

    app_state *appState = M_ArenaPush(&os->permanentArena, sizeof(app_state));

    GL_LoadProcedures();
    appState->spriteShader = R_InitSpriteShader();

    appState->sprite = R_CreateSprite("res/awesomeface.png");
}

APP_UPDATE
{
    local_persist f32 angle = 0;
    app_state *appState = os->permanentArena.base;

    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_WindowResize)
        {
            glViewport(0, 0, os->windowResolution.x, os->windowResolution.y);
            R_UpdateSpriteProjection(appState->spriteShader);
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
    R_DrawSprite(appState->spriteShader, appState->sprite, v2(0, 0), angle += 0.1f);
    OS_GLSwapBuffers();
}

APP_CLOSE
{
    OS_DebugPrint("APP_CLOSE\n");
}
