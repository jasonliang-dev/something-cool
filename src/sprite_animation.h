#pragma once

#include "geometry.h"
#include "language.h"

enum
{
    WALL_FOUNTAIN_MID_RED_ANIM,
    WALL_FOUNTAIN_BASIN_RED_ANIM,
    WALL_FOUNTAIN_MID_BLUE_ANIM,
    WALL_FOUNTAIN_BASIN_BLUE_ANIM,
    FLOOR_SPIKES_ANIM,
    CHEST_EMPTY_OPEN_ANIM,
    CHEST_FULL_OPEN_ANIM,
    CHEST_MIMIC_OPEN_ANIM,
    COIN_ANIM,
    TINY_ZOMBIE_IDLE_ANIM,
    TINY_ZOMBIE_RUN_ANIM,
    GOBLIN_IDLE_ANIM,
    GOBLIN_RUN_ANIM,
    IMP_IDLE_ANIM,
    IMP_RUN_ANIM,
    SKELET_IDLE_ANIM,
    SKELET_RUN_ANIM,
    MUDDY_IDLE_ANIM,
    MUDDY_RUN_ANIM,
    SWAMPY_IDLE_ANIM,
    SWAMPY_RUN_ANIM,
    ZOMBIE_IDLE_ANIM,
    ZOMBIE_RUN_ANIM,
    ICE_ZOMBIE_IDLE_ANIM,
    ICE_ZOMBIE_RUN_ANIM,
    MASKED_ORC_IDLE_ANIM,
    MASKED_ORC_RUN_ANIM,
    ORC_WARRIOR_IDLE_ANIM,
    ORC_WARRIOR_RUN_ANIM,
    ORC_SHAMAN_IDLE_ANIM,
    ORC_SHAMAN_RUN_ANIM,
    NECROMANCER_IDLE_ANIM,
    NECROMANCER_RUN_ANIM,
    WOGOL_IDLE_ANIM,
    WOGOL_RUN_ANIM,
    CHORT_IDLE_ANIM,
    CHORT_RUN_ANIM,
    BIG_ZOMBIE_IDLE_ANIM,
    BIG_ZOMBIE_RUN_ANIM,
    OGRE_IDLE_ANIM,
    OGRE_RUN_ANIM,
    BIG_DEMON_IDLE_ANIM,
    BIG_DEMON_RUN_ANIM,
    ELF_F_IDLE_ANIM,
    ELF_F_RUN_ANIM,
    ELF_F_HIT_ANIM,
    ELF_M_IDLE_ANIM,
    ELF_M_RUN_ANIM,
    ELF_M_HIT_ANIM,
    KNIGHT_F_IDLE_ANIM,
    KNIGHT_F_RUN_ANIM,
    KNIGHT_F_HIT_ANIM,
    KNIGHT_M_IDLE_ANIM,
    KNIGHT_M_RUN_ANIM,
    KNIGHT_M_HIT_ANIM,
    WIZZARD_F_IDLE_ANIM,
    WIZZARD_F_RUN_ANIM,
    WIZZARD_F_HIT_ANIM,
    WIZZARD_M_IDLE_ANIM,
    WIZZARD_M_RUN_ANIM,
    WIZZARD_M_HIT_ANIM,
    LIZARD_F_IDLE_ANIM,
    LIZARD_F_RUN_ANIM,
    LIZARD_F_HIT_ANIM,
    LIZARD_M_IDLE_ANIM,
    LIZARD_M_RUN_ANIM,
    LIZARD_M_HIT_ANIM,
    SPRITE_ANIMATION_MAX,
};

typedef struct SpriteAnimationEntry SpriteAnimationEntry;
struct SpriteAnimationEntry
{
    v4 rect;
    i32 frames;
};

static const SpriteAnimationEntry ANIMATION_TABLE[] = {
    [WALL_FOUNTAIN_MID_RED_ANIM] = {{64, 16, 16, 16}, 3},
    [WALL_FOUNTAIN_BASIN_RED_ANIM] = {{64, 32, 16, 16}, 3},
    [WALL_FOUNTAIN_MID_BLUE_ANIM] = {{64, 48, 16, 16}, 3},
    [WALL_FOUNTAIN_BASIN_BLUE_ANIM] = {{64, 64, 16, 16}, 3},
    [FLOOR_SPIKES_ANIM] = {{16, 176, 16, 16}, 4},
    [CHEST_EMPTY_OPEN_ANIM] = {{304, 288, 16, 16}, 3},
    [CHEST_FULL_OPEN_ANIM] = {{304, 304, 16, 16}, 3},
    [CHEST_MIMIC_OPEN_ANIM] = {{304, 320, 16, 16}, 3},
    [COIN_ANIM] = {{288, 272, 8, 8}, 4},
    [TINY_ZOMBIE_IDLE_ANIM] = {{368, 16, 16, 16}, 4},
    [TINY_ZOMBIE_RUN_ANIM] = {{432, 16, 16, 16}, 4},
    [GOBLIN_IDLE_ANIM] = {{368, 32, 16, 16}, 4},
    [GOBLIN_RUN_ANIM] = {{432, 32, 16, 16}, 4},
    [IMP_IDLE_ANIM] = {{368, 48, 16, 16}, 4},
    [IMP_RUN_ANIM] = {{432, 48, 16, 16}, 4},
    [SKELET_IDLE_ANIM] = {{368, 80, 16, 16}, 4},
    [SKELET_RUN_ANIM] = {{432, 80, 16, 16}, 4},
    [MUDDY_IDLE_ANIM] = {{368, 112, 16, 16}, 4},
    [MUDDY_RUN_ANIM] = {{368, 112, 16, 16}, 4},
    [SWAMPY_IDLE_ANIM] = {{432, 112, 16, 16}, 4},
    [SWAMPY_RUN_ANIM] = {{432, 112, 16, 16}, 4},
    [ZOMBIE_IDLE_ANIM] = {{368, 144, 16, 16}, 4},
    [ZOMBIE_RUN_ANIM] = {{368, 144, 16, 16}, 4},
    [ICE_ZOMBIE_IDLE_ANIM] = {{432, 144, 16, 16}, 4},
    [ICE_ZOMBIE_RUN_ANIM] = {{432, 144, 16, 16}, 4},
    [MASKED_ORC_IDLE_ANIM] = {{368, 172, 16, 20}, 4},
    [MASKED_ORC_RUN_ANIM] = {{432, 172, 16, 20}, 4},
    [ORC_WARRIOR_IDLE_ANIM] = {{368, 204, 16, 20}, 4},
    [ORC_WARRIOR_RUN_ANIM] = {{432, 204, 16, 20}, 4},
    [ORC_SHAMAN_IDLE_ANIM] = {{368, 236, 16, 20}, 4},
    [ORC_SHAMAN_RUN_ANIM] = {{432, 236, 16, 20}, 4},
    [NECROMANCER_IDLE_ANIM] = {{368, 268, 16, 20}, 4},
    [NECROMANCER_RUN_ANIM] = {{368, 268, 16, 20}, 4},
    [WOGOL_IDLE_ANIM] = {{368, 300, 16, 20}, 4},
    [WOGOL_RUN_ANIM] = {{432, 300, 16, 20}, 4},
    [CHORT_IDLE_ANIM] = {{368, 328, 16, 24}, 4},
    [CHORT_RUN_ANIM] = {{432, 328, 16, 24}, 4},
    [BIG_ZOMBIE_IDLE_ANIM] = {{16, 270, 32, 34}, 4},
    [BIG_ZOMBIE_RUN_ANIM] = {{144, 270, 32, 34}, 4},
    [OGRE_IDLE_ANIM] = {{16, 320, 32, 32}, 4},
    [OGRE_RUN_ANIM] = {{144, 320, 32, 32}, 4},
    [BIG_DEMON_IDLE_ANIM] = {{16, 364, 32, 36}, 4},
    [BIG_DEMON_RUN_ANIM] = {{144, 364, 32, 36}, 4},
    [ELF_F_IDLE_ANIM] = {{128, 4, 16, 28}, 4},
    [ELF_F_RUN_ANIM] = {{192, 4, 16, 28}, 4},
    [ELF_F_HIT_ANIM] = {{256, 4, 16, 28}, 1},
    [ELF_M_IDLE_ANIM] = {{128, 36, 16, 28}, 4},
    [ELF_M_RUN_ANIM] = {{192, 36, 16, 28}, 4},
    [ELF_M_HIT_ANIM] = {{256, 36, 16, 28}, 1},
    [KNIGHT_F_IDLE_ANIM] = {{128, 68, 16, 28}, 4},
    [KNIGHT_F_RUN_ANIM] = {{192, 68, 16, 28}, 4},
    [KNIGHT_F_HIT_ANIM] = {{256, 68, 16, 28}, 1},
    [KNIGHT_M_IDLE_ANIM] = {{128, 100, 16, 28}, 4},
    [KNIGHT_M_RUN_ANIM] = {{192, 100, 16, 28}, 4},
    [KNIGHT_M_HIT_ANIM] = {{256, 100, 16, 28}, 1},
    [WIZZARD_F_IDLE_ANIM] = {{128, 132, 16, 28}, 4},
    [WIZZARD_F_RUN_ANIM] = {{192, 132, 16, 28}, 4},
    [WIZZARD_F_HIT_ANIM] = {{256, 132, 16, 28}, 1},
    [WIZZARD_M_IDLE_ANIM] = {{128, 164, 16, 28}, 4},
    [WIZZARD_M_RUN_ANIM] = {{192, 164, 16, 28}, 4},
    [WIZZARD_M_HIT_ANIM] = {{256, 164, 16, 28}, 1},
    [LIZARD_F_IDLE_ANIM] = {{128, 196, 16, 28}, 4},
    [LIZARD_F_RUN_ANIM] = {{192, 196, 16, 28}, 4},
    [LIZARD_F_HIT_ANIM] = {{256, 196, 16, 28}, 1},
    [LIZARD_M_IDLE_ANIM] = {{128, 228, 16, 28}, 4},
    [LIZARD_M_RUN_ANIM] = {{192, 228, 16, 28}, 4},
    [LIZARD_M_HIT_ANIM] = {{256, 228, 16, 28}, 1},
};

typedef struct SpriteAnimation SpriteAnimation;
struct SpriteAnimation
{
    v4 rect;
    i32 frames;
    i32 msPerFrame;
    f32 elapsedTime;
};

SpriteAnimation CreateSpriteAnimation(i32 name);
void ResetSpriteAnimation(SpriteAnimation *animation);
void UpdateSpriteAnimation(SpriteAnimation *animation, f32 deltaTime);
void DrawSpriteAnimation(const SpriteAnimation *animation, v2 pos);
void DrawSpriteAnimationExt(const SpriteAnimation *animation, v2 pos, v2 scale, v4 color);
