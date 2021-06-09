#include "scenes.h"

void name##EnterScene(void)
{
}

i32 name##UpdateScene(f32 deltaTime)
{
    (void)deltaTime;

    i32 scene = SCENE_name;

    return scene;
}

void name##DrawScene(m4 projection)
{
    (void)projection;
}

void name##ExitScene(void)
{
}
