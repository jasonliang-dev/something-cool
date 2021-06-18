#include "sprite_animation.h"

typedef struct PlayerAppearance PlayerAppearance;
struct PlayerAppearance
{
    i32 idle;
    i32 run;
};

enum PlayerAppearanceType
{
    PLAYER_APPEARANCE_ELF_M,
    PLAYER_APPEARANCE_ELF_F,
    PLAYER_APPEARANCE_KNIGHT_M,
    PLAYER_APPEARANCE_KNIGHT_F,
    PLAYER_APPEARANCE_WIZZARD_M,
    PLAYER_APPEARANCE_WIZZARD_F,
    PLAYER_APPEARANCE_LIZARD_M,
    PLAYER_APPEARANCE_LIZARD_F,
    PLAYER_APPEARANCE_MAX,
};

static PlayerAppearance PLAYER_APPEARANCE_TABLE[] = {
    [PLAYER_APPEARANCE_ELF_M] =
        {
            .idle = ELF_M_IDLE_ANIM,
            .run = ELF_M_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_ELF_F] =
        {
            .idle = ELF_F_IDLE_ANIM,
            .run = ELF_F_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_KNIGHT_M] =
        {
            .idle = KNIGHT_M_IDLE_ANIM,
            .run = KNIGHT_M_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_KNIGHT_F] =
        {
            .idle = KNIGHT_F_IDLE_ANIM,
            .run = KNIGHT_F_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_WIZZARD_M] =
        {
            .idle = WIZZARD_M_IDLE_ANIM,
            .run = WIZZARD_M_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_WIZZARD_F] =
        {
            .idle = WIZZARD_F_IDLE_ANIM,
            .run = WIZZARD_F_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_LIZARD_M] =
        {
            .idle = LIZARD_M_IDLE_ANIM,
            .run = LIZARD_M_RUN_ANIM,
        },
    [PLAYER_APPEARANCE_LIZARD_F] =
        {
            .idle = LIZARD_F_IDLE_ANIM,
            .run = LIZARD_F_RUN_ANIM,
        },
};

static SpriteAnimation PlayerAnimateIdle(i32 type)
{
    return CreateSpriteAnimation(PLAYER_APPEARANCE_TABLE[type].idle, 150);
}

static SpriteAnimation PlayerAnimateRun(i32 type)
{
    return CreateSpriteAnimation(PLAYER_APPEARANCE_TABLE[type].run, 100);
}

static SpriteAnimation PlayerAnimateDash(i32 type)
{
    return CreateSpriteAnimation(PLAYER_APPEARANCE_TABLE[type].run, 50);
}
