#include "scenes.h"

typedef struct SceneVTable SceneVTable;
struct SceneVTable
{
    void (*Enter)(void);
    i32 (*Update)(f32 deltaTime);
    void (*Draw)(m4 projection);
    void (*Exit)(void);
};

#define X(en, name)                                                                      \
    [en] = {                                                                             \
        .Enter = name##EnterScene,                                                       \
        .Update = name##UpdateScene,                                                     \
        .Draw = name##DrawScene,                                                         \
        .Exit = name##ExitScene,                                                         \
    },

static SceneVTable g_SceneVTables[] = {SCENE_LIST};

#undef X

static struct
{
    i32 current;
    i32 next;
} g_Scene;

void InitScenes(i32 initialScene)
{
    g_Scene.current = initialScene;
    g_Scene.next = initialScene;

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
