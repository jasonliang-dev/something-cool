#include "scenes.h"
#include "ui.h"

void SetupPortEnterScene(void)
{
}

i32 SetupPortUpdateScene(f32 deltaTime)
{
    (void)deltaTime;

    i32 scene = SCENE_SETUP_PORT;

    UIBegin();

    if (UITextButton(UIID(), "Back", v2(0, 0)))
    {
        scene = SCENE_MAIN_MENU;
    }

    return scene;
}

void SetupPortDrawScene(m4 projection)
{
    DrawUI(projection);
}

void SetupPortExitScene(void)
{
}
