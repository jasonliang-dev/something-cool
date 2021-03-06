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

static const v3 SCALE = {3, 3, 1};

static b32 g_Connected;
static u32 g_ID;
static Player g_Player;
static v2 g_Camera;
static RemotePlayer g_RemotePlayers[MAX_REMOTE_PLAYERS];
static NetPlayerInfo g_RemoteInfo[MAX_REMOTE_PLAYERS];

static void OnRecieve(ENetEvent event)
{
    NetMessage *msg = (NetMessage *)event.packet->data;
    switch (msg->type)
    {
    case TO_CLIENT_PLAYER_ID:
        printf("playerID: %d\n", msg->id);
        g_ID = msg->id;
        break;
    case TO_CLIENT_PLAYER_LIST:
        printf("Player List:");

        RemotePlayer *begin = g_RemotePlayers;
        for (i32 i = 0; i < msg->playerList.count; ++i)
        {
            printf(" %d", msg->playerList.ids[i]);
            if (msg->playerList.ids[i] != g_ID)
            {
                *begin++ = CreateRemotePlayer(msg->playerList.ids[i]);
            }
        }

        printf("\n");
        break;
    case TO_CLIENT_PLAYER_INFO:
        for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
        {
            if (g_RemotePlayers[i].id == msg->id)
            {
                g_RemoteInfo[i] = msg->playerInfo;
                break;
            }
        }
        break;
    case TO_CLIENT_PLAYER_JOIN:
        printf("Player with id %d joined\n", msg->id);

        for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
        {
            if (!g_RemotePlayers[i].id)
            {
                g_RemotePlayers[i] = CreateRemotePlayer(msg->id);
                break;
            }
        }
        break;
    case TO_CLIENT_PLAYER_LEAVE:
        printf("Player with id %d left\n", msg->id);
        for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
        {
            if (g_RemotePlayers[i].id == msg->id)
            {
                g_RemotePlayers[i].id = 0;
                break;
            }
        }
        break;
    default:
        printf("Ignoring packet with type: %d\n", msg->type);
        break;
    }
}

static void OnDisconnect(void)
{
    g_Connected = false;
}

void LobbyEnterScene(void)
{
    g_Connected = true;
    g_ID = 0;
    g_Player = CreatePlayer(v2(50, 50));
    g_Camera = g_Player.pos;
    memset(g_RemotePlayers, 0, sizeof(g_RemotePlayers));

    ClientReceiveCallback(OnRecieve);
    ClientOnDisconnect(OnDisconnect);
    ClientSend((NetMessage){.type = TO_SERVER_PLAYER_ID});
    ClientSend((NetMessage){.type = TO_SERVER_PLAYER_LIST});

    glfwSetInputMode(g_Window.handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

i32 LobbyUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_LOBBY;

    if (KeyPressed(GLFW_KEY_ESCAPE))
    {
        scene = SCENE_MAIN_MENU;
    }

    if (!g_Connected)
    {
        scene = SCENE_MAIN_MENU;
    }

    for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
    {
        if (g_RemotePlayers[i].id)
        {
            UpdateRemotePlayer(g_RemotePlayers + i, g_RemoteInfo[i], deltaTime);
        }
    }

    UpdatePlayer(&g_Player, &map_Test, deltaTime);

    v2 mouse = MousePos();
    v2 halfWin = v2(g_Window.width / 2.0f, g_Window.height / 2.0f);
    v2 target =
        v2((g_Player.pos.x * SCALE.x) + ((mouse.x - halfWin.x) / SCALE.x) - halfWin.x,
           (g_Player.pos.y * SCALE.y) + ((mouse.y - halfWin.y) / SCALE.y * 1.1f) -
               halfWin.y);

    static const f32 CAMERA_SPEED = 4.0f;
    g_Camera.x += (target.x - g_Camera.x) * CAMERA_SPEED * deltaTime;
    g_Camera.y += (target.y - g_Camera.y) * CAMERA_SPEED * deltaTime;

    return scene;
}

void LobbyDrawScene(m4 projection)
{
    m4 view = M4Translate(m4(1), v3(-g_Camera.x, -g_Camera.y, 0));
    view = M4Scale(view, SCALE);

    BeginDraw(M4xM4(projection, view));
    DrawTilemap(map_Test);

    for (i32 i = 0; i < MAX_REMOTE_PLAYERS; ++i)
    {
        if (g_RemotePlayers[i].id)
        {
            DrawRemotePlayer(g_RemotePlayers + i);
        }
    }

    DrawPlayer(&g_Player);
    EndDraw();

    BeginDraw(M4Scale(projection, SCALE));
    DrawCursor(v2(SCALE.x, SCALE.y));
    EndDraw();
}

void LobbyExitScene(void)
{
    glfwSetInputMode(g_Window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    CloseServer();
    ClientDisconnect();
}
