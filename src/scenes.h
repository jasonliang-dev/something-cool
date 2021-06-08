#pragma once

#include "language.h"
#include "geometry.h"

enum
{
    SCENE_MAIN_MENU,
    SCENE_TEST,
    SCENE_MAX,
};

void InitScenes(i32 initialScene);
void UpdateScene(f32 deltaTime);
void DrawScene(m4 projection);
void SceneFinishFrame(void);

void MainMenuEnterScene(void);
i32 MainMenuUpdateScene(f32 deltaTime);
void MainMenuDrawScene(m4 projection);
void MainMenuExitScene(void);

void TestEnterScene(void);
i32 TestUpdateScene(f32 deltaTime);
void TestDrawScene(m4 projection);
void TestExitScene(void);
