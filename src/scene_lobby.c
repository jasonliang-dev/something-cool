#include "scenes.h"
#include "input.h"
#include "renderer.h"
#include "assets.h"
#include "net.h"

void LobbyEnterScene(void)
{
}

i32 LobbyUpdateScene(f32 deltaTime)
{
    (void)deltaTime;

    i32 scene = SCENE_LOBBY;

    if (KeyPressed(GLFW_KEY_ESCAPE))
    {
        scene = SCENE_MAIN_MENU;
    }

    return scene;
}

void LobbyDrawScene(m4 projection)
{
    BeginDraw(tex_Atlas, M4xM4(projection, M4Scale(m4(1), v3(3, 3, 1))));
    DrawTilemap(map_Test);
    EndDraw();
}

void LobbyExitScene(void)
{
    CloseServer();
    ClientDisconnect();
}
