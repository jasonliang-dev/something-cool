#include "net.h"
#include "scenes.h"
#include "ui.h"

enum JoinServerState
{
    JOIN_SERVER_INPUT,
    JOIN_SERVER_CONNECTING,
    JOIN_SERVER_FAILED,
    JOIN_SERVER_SUCCESS,
};

static enum JoinServerState g_State;

void JoinServerEnterScene(void)
{
    g_State = JOIN_SERVER_INPUT;
}

static void OnConnect(NetError err)
{
    if (err)
    {
        printf("NetError: %s\n", err);
        g_State = JOIN_SERVER_FAILED;
        return;
    }

    g_State = JOIN_SERVER_SUCCESS;
}

i32 JoinServerUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_JOIN_SERVER;

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 32), 20, UI_AUTO_LAYOUT_COLUMN);

    switch (g_State)
    {
    case JOIN_SERVER_INPUT: {
        static char s_Host[128] = "127.0.0.1";
        static char s_Port[16] = "4242";

        UIInputText(UIID(), s_Host, sizeof(s_Host), v2(0, 0), 0);
        UIInputText(UIID(), s_Port, sizeof(s_Port), v2(0, 0), UI_INPUT_NUMERIC);

        if (UITextButton(UIID(), "Join", v2(0, 0)))
        {
            ClientConnect(s_Host, (u16)atoi(s_Port), OnConnect);
            g_State = JOIN_SERVER_CONNECTING;
        }

        if (UITextButton(UIID(), "Back", v2(0, 0)))
        {
            scene = SCENE_MAIN_MENU;
        }
        break;
    }
    case JOIN_SERVER_CONNECTING: {
        UILabel(UIID(), "Connecting...", v2(0, 0));

        if (UITextButton(UIID(), "Back", v2(0, 0)))
        {
            ClientForceDisconnect();
            g_State = JOIN_SERVER_INPUT;
        }
        break;
    }
    case JOIN_SERVER_FAILED: {
        UILabel(UIID(), "Connection failed", v2(0, 0));

        if (UITextButton(UIID(), "Back", v2(0, 0)))
        {
            ClientForceDisconnect();
            g_State = JOIN_SERVER_INPUT;
        }
        break;
    }
    case JOIN_SERVER_SUCCESS: {
        scene = SCENE_LOBBY;
        break;
    }
    }

    return scene;
}

void JoinServerDrawScene(m4 projection)
{
    DrawUI(projection);
}

void JoinServerExitScene(void)
{
}
