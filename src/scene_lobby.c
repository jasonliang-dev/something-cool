#include "scenes.h"
#include "input.h"
#include "renderer.h"
#include "assets.h"
#include "net.h"
#include "player.h"
#include "cursor.h"
#include "window.h"

static Player s_Player;

void LobbyEnterScene(void)
{
    s_Player = CreatePlayer(v2(50, 50));
    glfwSetInputMode(g_Window.handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

i32 LobbyUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_LOBBY;

    if (KeyPressed(GLFW_KEY_ESCAPE))
    {
        scene = SCENE_MAIN_MENU;
    }

    UpdatePlayer(&s_Player, &map_Test, deltaTime);

    return scene;
}

void LobbyDrawScene(m4 projection)
{
    BeginDraw(M4xM4(projection, M4Scale(m4(1), v3(3, 3, 1))));
    DrawTilemap(map_Test);
    DrawPlayer(&s_Player);
    DrawCursor(v2(3, 3));
    EndDraw();
}

void LobbyExitScene(void)
{
    glfwSetInputMode(g_Window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    CloseServer();
    ClientDisconnect();
}
