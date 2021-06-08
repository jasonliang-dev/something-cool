#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS

#include "scenes.h"
#include "input.h"
#include <cimgui.h>

void MainMenuEnterScene(void)
{
}

i32 MainMenuUpdateScene(f32 deltaTime)
{
    (void)deltaTime;

    i32 scene = SCENE_MAIN_MENU;

    if (KeyPressed(GLFW_KEY_SPACE) || KeyPressed(GLFW_KEY_ENTER))
    {
        scene = SCENE_TEST;
    }

    igBegin("Main Menu", NULL, 0);
    if (igButton("Go to test scene", (ImVec2){0, 0}))
    {
        scene = SCENE_TEST;
    }
    igEnd();

    return scene;
}

void MainMenuDrawScene(m4 projection)
{
    (void)projection;
}

void MainMenuExitScene(void)
{
}
