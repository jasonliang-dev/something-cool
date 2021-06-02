#pragma once

#include "language.h"
#include "geometry.h"
#include "sprite_animation.h"

typedef enum
{
    PLAYER_IDLE,
    PLAYER_RUN,
    PLAYER_DASH,
    PLAYER_STATE_MAX,
} PlayerState;

enum
{
    PLAYER_FACING_LEFT = 1 << 0,
};

typedef struct Player Player;
struct Player
{
    u32 flags;
    u32 state;
    v2 pos;
    v2 vel;
    f32 moveSpeed;
    f32 dashTime;
    SpriteAnimation animation;
};

Player CreatePlayer(void);

void UpdatePlayer(Player *player, f32 deltaTime);
void DrawPlayer(const Player *player);
