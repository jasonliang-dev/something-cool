#pragma once

#include "geometry.h"

#define SCENE_LIST                                                                       \
    X(SCENE_CUSTOMIZE, Customize)                                                        \
    X(SCENE_MAIN_MENU, MainMenu)                                                         \
    X(SCENE_SETUP_PORT, SetupPort)                                                       \
    X(SCENE_JOIN_SERVER, JoinServer)                                                     \
    X(SCENE_LOBBY, Lobby)                                                                \
    X(SCENE_OPTIONS, Options)                                                            \
    X(SCENE_TEST, Test)

#define X(en, _) en,
enum SceneType
{
    SCENE_NULL,
    SCENE_LIST
};
#undef X

void InitScenes(i32 initialScene);
void UpdateScene(f32 deltaTime);
void DrawScene(m4 projection);
void SceneFinishFrame(void);

#define X(_, name)                                                                       \
    void name##EnterScene(void);                                                         \
    i32 name##UpdateScene(f32 deltaTime);                                                \
    void name##DrawScene(m4 projection);                                                 \
    void name##ExitScene(void);

SCENE_LIST

#undef X
