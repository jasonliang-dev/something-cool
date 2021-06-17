#include "assets.h"
#include "cursor.h"
#include "input.h"
#include "net.h"
#include "player.h"
#include "remote_player.h"
#include "renderer.h"
#include "scenes.h"
#include "window.h"
#include <string.h>

#define MAX_REMOTE_PLAYERS 32

static u32 s_ID;
static Player s_Player;
static RemotePlayer s_RemotePlayers[MAX_REMOTE_PLAYERS];

static void OnRecieve(ENetEvent event)
{
    NetMessage *msg = (NetMessage *)event.packet->data;
    switch (msg->type)
    {
    case TO_CLIENT_PLAYER_ID:
        printf("playerID: %d\n", msg->id);
        s_ID = msg->id;
        break;
    case TO_CLIENT_PLAYER_LIST:
        printf("Player List:");

        RemotePlayer *begin = s_RemotePlayers;
        for (i32 i = 0; i < msg->playerList.count; ++i)
        {
            printf(" %d", msg->playerList.ids[i]);
            if (msg->playerList.ids[i] != s_ID)
            {
                *begin++ = CreateRemotePlayer(msg->playerList.ids[i]);
            }
        }

        printf("\n");
        break;
    case TO_CLIENT_PLAYER_INFO:
        for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
        {
            if (s_RemotePlayers[i].id == msg->id)
            {
                UpdateRemotePlayer(s_RemotePlayers + i, msg->playerInfo);
                break;
            }
        }
        break;
    case TO_CLIENT_PLAYER_JOIN:
        printf("Player with id %d joined\n", msg->id);

        for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
        {
            if (!s_RemotePlayers[i].id)
            {
                s_RemotePlayers[i] = CreateRemotePlayer(msg->id);
            }
        }
        break;
    default:
        printf("Ignoring packet with type: %d\n", msg->type);
        break;
    }
}

void LobbyEnterScene(void)
{
    ClientReceiveCallback(OnRecieve);
    ClientSend((NetMessage){.type = TO_SERVER_PLAYER_ID});
    ClientSend((NetMessage){.type = TO_SERVER_PLAYER_LIST});
    s_ID = 0;
    s_Player = CreatePlayer(v2(50, 50));
    memset(s_RemotePlayers, 0, sizeof(s_RemotePlayers));
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

    for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
    {
        if (s_RemotePlayers[i].id)
        {
            DrawRemotePlayer(s_RemotePlayers + i);
        }
    }

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
