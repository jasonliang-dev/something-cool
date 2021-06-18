#include "player_appearance.h"
#include "renderer.h"
#include "scenes.h"
#include "ui.h"

static SpriteAnimation g_PlayerAnimations[PLAYER_APPEARANCE_MAX];
static i32 g_Appearance;

void CustomizeEnterScene(void)
{
    g_Appearance = 0;

    for (i32 i = 0; i < PLAYER_APPEARANCE_MAX; ++i)
    {
        g_PlayerAnimations[i] = PlayerAnimateRun(i);
    }
}

i32 CustomizeUpdateScene(f32 deltaTime)
{
    i32 scene = SCENE_CUSTOMIZE;

    UpdateSpriteAnimation(g_PlayerAnimations + g_Appearance, deltaTime);

    UIBegin(deltaTime);
    UIPushAutoLayout(v2(32, 100), 20, UI_AUTO_LAYOUT_COLUMN);

    static char s_Name[32] = "";
    UIInputText(UIID(), s_Name, sizeof(s_Name), v2(0, 0), 0);

    if (UITextButton(UIID(), "Prev", v2(0, 0)))
    {
        g_Appearance--;
        if (g_Appearance < 0)
        {
            g_Appearance = PLAYER_APPEARANCE_MAX - 1;
        }
    }

    if (UITextButton(UIID(), "Next", v2(0, 0)))
    {
        g_Appearance++;
        if (g_Appearance >= PLAYER_APPEARANCE_MAX)
        {
            g_Appearance = 0;
        }
    }

    if (UITextButton(UIID(), "Done", v2(0, 0)))
    {
        scene = SCENE_MAIN_MENU;
    }

    return scene;
}

void CustomizeDrawScene(m4 projection)
{
    DrawUI(projection);

    BeginDraw(M4Scale(projection, v3(3, 3, 1)));
    DrawSpriteAnimation(g_PlayerAnimations + g_Appearance, v2(32, 16));
    EndDraw();
}

void CustomizeExitScene(void)
{
}
