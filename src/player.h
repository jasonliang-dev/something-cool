#pragma once

#include "geometry.h"
#include "language.h"
#include "sprite_animation.h"
#include "tilemap.h"
#include "player_ghosts.h"

typedef struct Player Player;
struct Player
{
    i32 flags;
    i32 state;
    i32 appearanceType;
    v4 rect;
    v2 pos;
    v2 vel;
    f32 moveSpeed;
    f32 dashTime;
    SpriteAnimation animation;
    PlayerGhosts ghosts;
};

Player CreatePlayer(v2 pos);

void UpdatePlayer(Player *player, const Tilemap *map, f32 deltaTime);
void DrawPlayer(const Player *player);
