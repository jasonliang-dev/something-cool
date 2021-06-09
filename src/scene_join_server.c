#include "net.h"
#include "scenes.h"
#include "ui.h"

void JoinServerEnterScene(void)
{
}

i32 JoinServerUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_JOIN_SERVER;

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 32), 20, UI_AUTO_LAYOUT_COLUMN);

    static char s_Host[128] = "127.0.0.1";
    static char s_Port[16] = "4242";

    UIInputText(UIID(), s_Host, sizeof(s_Host), v2(0, 0), 0);
    UIInputText(UIID(), s_Port, sizeof(s_Port), v2(0, 0), UI_INPUT_NUMERIC);

    if (UITextButton(UIID(), "Join", v2(0, 0)))
    {
        NetError err = ClientConnect(s_Host, (u16)atoi(s_Port));
        if (err)
        {
            fprintf(stderr, "error: %s\n", err);
        }
        else
        {
            scene = SCENE_LOBBY;
        }
    }

    if (UITextButton(UIID(), "Back", v2(0, 0)))
    {
        scene = SCENE_MAIN_MENU;
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
