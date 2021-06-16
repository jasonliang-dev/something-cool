#include "net.h"
#include "scenes.h"
#include "ui.h"

enum SetupPortState
{
    SETUP_PORT_INPUT,
    SETUP_PORT_CONNECTING,
    SETUP_PORT_FAILED,
    SETUP_PORT_SUCCESS,
};

static enum SetupPortState g_State;
static NetError g_ConnectError;

void SetupPortEnterScene(void)
{
    g_State = SETUP_PORT_INPUT;
    g_ConnectError = NULL;
}

static void OnConnect(NetError err)
{
    if (err)
    {
        g_ConnectError = err;
        printf("NetError: %s\n", err);
        return;
    }

    g_State = SETUP_PORT_SUCCESS;
}

i32 SetupPortUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_SETUP_PORT;

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 32), 20, UI_AUTO_LAYOUT_COLUMN);

    switch (g_State)
    {
    case SETUP_PORT_INPUT: {
        static char s_Port[16] = "4242";
        UIInputText(UIID(), s_Port, sizeof(s_Port), v2(0, 0), UI_INPUT_NUMERIC);

        if (UITextButton(UIID(), "Host Server", v2(0, 0)))
        {
            u16 port = (u16)atoi(s_Port);
            NetError err = InitServer(port);

            if (err != NULL)
            {
                g_ConnectError = err;
                g_State = SETUP_PORT_FAILED;
            }

            ClientConnect("127.0.0.1", port, OnConnect);
        }

        if (UITextButton(UIID(), "Back", v2(0, 0)))
        {
            scene = SCENE_MAIN_MENU;
        }
        break;
    }
    case SETUP_PORT_CONNECTING: {
        UILabel(UIID(), "Connecting...", v2(0, 0));

        if (UITextButton(UIID(), "Back", v2(0, 0)))
        {
            CloseServer();
            ClientForceDisconnect();
            g_State = SETUP_PORT_INPUT;
        }
        break;
    }
    case SETUP_PORT_FAILED: {
        UILabel(UIID(), "Failed to start host", v2(0, 0));

        if (UITextButton(UIID(), "Back", v2(0, 0)))
        {
            CloseServer();
            ClientForceDisconnect();
            g_State = SETUP_PORT_INPUT;
        }
        break;
    }
    case SETUP_PORT_SUCCESS: {
        scene = SCENE_LOBBY;
        break;
    }
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
