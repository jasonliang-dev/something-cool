#include "scenes.h"

typedef struct SceneVTable SceneVTable;
struct SceneVTable
{
    void (*Enter)(void);
    i32 (*Update)(f32 deltaTime);
    void (*Draw)(m4 projection);
    void (*Exit)(void);
};

static SceneVTable g_SceneVTables[SCENE_MAX] = {
    [SCENE_MAIN_MENU] =
        {
            .Enter = MainMenuEnterScene,
            .Update = MainMenuUpdateScene,
            .Draw = MainMenuDrawScene,
            .Exit = MainMenuExitScene,
        },
    [SCENE_TEST] =
        {
            .Enter = TestEnterScene,
            .Update = TestUpdateScene,
            .Draw = TestDrawScene,
            .Exit = TestExitScene,
        },
};

static struct
{
    i32 current;
    i32 next;
} g_Scene;

void InitScenes(i32 initialScene)
{
    g_Scene.current = initialScene;
    g_Scene.next = SCENE_MAX;

    g_SceneVTables[g_Scene.current].Enter();
}

void UpdateScene(f32 deltaTime)
{
    g_Scene.next = g_SceneVTables[g_Scene.current].Update(deltaTime);
}

void DrawScene(m4 projection)
{
    g_SceneVTables[g_Scene.current].Draw(projection);
}

void SceneFinishFrame(void)
{
    if (g_Scene.next == g_Scene.current)
    {
        return;
    }

    g_SceneVTables[g_Scene.current].Exit();
    g_SceneVTables[g_Scene.next].Enter();

    g_Scene.current = g_Scene.next;
}
