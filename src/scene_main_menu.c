#include "scenes.h"
#include "ui.h"

void MainMenuEnterScene(void)
{
}

i32 MainMenuUpdateScene(f32 deltaTime)
{
    (void)deltaTime;

    i32 scene = SCENE_MAIN_MENU;

    enum
    {
        PAD_X = 0,
        GAP_Y = 50,
    };

    UIBegin();

    f32 y = 0.0f;
    if (UITextButton(UIID(), "Host", v2(PAD_X, y)))
    {
        scene = SCENE_SETUP_PORT;
    }

    y += GAP_Y;
    if (UITextButton(UIID(), "Join", v2(PAD_X, y)))
    {
        //
    }

    y += GAP_Y;
    if (UITextButton(UIID(), "Options", v2(PAD_X, y)))
    {
        //
    }

    y += GAP_Y;
    if (UITextButton(UIID(), "Quit", v2(PAD_X, y)))
    {
        exit(0);
    }

    y += GAP_Y;
    if (UITextButton(UIID(), "Test Scene", v2(PAD_X, y)))
    {
        scene = SCENE_TEST;
    }

    return scene;
}

void MainMenuDrawScene(m4 projection)
{
    DrawUI(projection);
}

void MainMenuExitScene(void)
{
}
