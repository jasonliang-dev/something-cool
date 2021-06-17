#include "assets.h"
#include "input.h"
#include "net.h"
#include "player.h"
#include "scenes.h"

static Player player;
static SpriteAnimation ani;

void TestEnterScene(void)
{
    player = CreatePlayer(v2(50, 50));
    ani = CreateSpriteAnimation(BIG_DEMON_IDLE_ANIM);
}

i32 TestUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_TEST;

    if (KeyPressed(GLFW_KEY_ESCAPE))
    {
        scene = SCENE_MAIN_MENU;
    }

    if (KeyPressed(GLFW_KEY_F4))
    {
        // ClientSend("packet");
    }

    UpdatePlayer(&player, &map_Test, deltaTime);
    UpdateSpriteAnimation(&ani, deltaTime);

    //

    /*
    static bool s_Demo = false;
    static char s_ServerPort[16] = "4242";
    static char s_RemoteHost[256] = "127.0.0.1";
    static char s_RemotePort[16] = "4242";

    igBegin("Debug Window", NULL, 0);

    igInputText("Server Port", s_ServerPort, ArrayCount(s_ServerPort),
                ImGuiInputTextFlags_CharsDecimal, 0, 0);
    if (igButton("Create server host", (ImVec2){0, 0}))
    {
        InitServer((u16)atoi(s_ServerPort));
    }

    igInputText("Remote Host", s_RemoteHost, ArrayCount(s_RemoteHost), 0, 0, 0);
    igInputText("Port", s_RemotePort, ArrayCount(s_RemotePort),
                ImGuiInputTextFlags_CharsDecimal, 0, 0);
    if (igButton("Connect to host", (ImVec2){0, 0}))
    {
        NetError err = ClientConnect(s_RemoteHost, (u16)atoi(s_RemotePort));
        if (err)
        {
            fprintf(stderr, "error: %s\n", err);
        }
    }

    igCheckbox("Demo Window", &s_Demo);
    igText("Application average %.4f ms/frame (%.1f FPS)", deltaTime, 1 / deltaTime);
    igEnd();

    if (s_Demo)
    {
        igShowDemoWindow(&s_Demo);
    }
    */

    return scene;
}

void TestDrawScene(m4 projection)
{
    BeginDraw(M4xM4(projection, M4Scale(m4(1), v3(3, 3, 1))));
    DrawTilemap(map_Test);
    DrawSpriteAnimation(&ani, v2(100, 100));
    DrawPlayer(&player);
    EndDraw();

    BeginDraw(projection);

    m4 transform = M4Scale(
        m4(1), v3((f32)fnt_Primary.texture.width, (f32)fnt_Primary.texture.height, 1));
    DrawQuad(transform, v4(0, 0, 1, 1), fnt_Primary.texture.id, v4(1, 0, 0, 1));

    DrawFont("Hello World", fnt_Primary, v2(50, 50), v4(1, 1, 1, 1));
    EndDraw();
}

void TestExitScene(void)
{
}
