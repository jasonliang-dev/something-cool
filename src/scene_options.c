#include "scenes.h"
#include "ui.h"

void OptionsEnterScene(void)
{
}

i32 OptionsUpdateScene(f32 deltaTime)
{
    (void)deltaTime;

    i32 scene = SCENE_OPTIONS;

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 32), 20, UI_AUTO_LAYOUT_COLUMN);

    if (UITextButton(UIID(), "Test Scene", v2(0, 0)))
    {
        scene = SCENE_TEST;
    }

    if (UITextButton(UIID(), "Customize", v2(0, 0)))
    {
        scene = SCENE_CUSTOMIZE;
    }

    if (UITextButton(UIID(), "Back", v2(0, 0)))
    {
        scene = SCENE_MAIN_MENU;
    }

    return scene;
}

void OptionsDrawScene(m4 projection)
{
    DrawUI(projection);
}

void OptionsExitScene(void)
{
}
