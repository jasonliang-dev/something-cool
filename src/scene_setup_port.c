#include "net.h"
#include "scenes.h"
#include "ui.h"

void SetupPortEnterScene(void)
{
}

i32 SetupPortUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_SETUP_PORT;

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 32), 20, UI_AUTO_LAYOUT_COLUMN);

    static char s_Port[16] = "4242";
    UIInputText(UIID(), s_Port, sizeof(s_Port), v2(0, 0), UI_INPUT_NUMERIC);

    if (UITextButton(UIID(), "Host Server", v2(0, 0)))
    {
        u16 port = (u16)atoi(s_Port);
        NetError err;

        if ((err = InitServer(port)) != NULL)
        {
            fprintf(stderr, "InitServer: %s\n", err);
        }
        else if ((err = ClientConnect("127.0.0.1", port)) != NULL)
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

void SetupPortDrawScene(m4 projection)
{
    DrawUI(projection);
}

void SetupPortExitScene(void)
{
}
