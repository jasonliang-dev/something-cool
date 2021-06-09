#include "scenes.h"
#include "ui.h"

void MainMenuEnterScene(void)
{
}

i32 MainMenuUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_MAIN_MENU;

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 32), 20, UI_AUTO_LAYOUT_COLUMN);

    if (UITextButton(UIID(), "Host", v2(0, 0)))
    {
        scene = SCENE_SETUP_PORT;
    }

    if (UITextButton(UIID(), "Join", v2(0, 0)))
    {
        scene = SCENE_JOIN_SERVER;
    }

    if (UITextButton(UIID(), "Options", v2(0, 0)))
    {
        scene = SCENE_OPTIONS;
    }

    if (UITextButton(UIID(), "Quit", v2(0, 0)))
    {
        exit(0);
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
