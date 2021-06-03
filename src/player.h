#pragma once

#include "geometry.h"
#include "language.h"
#include "sprite_animation.h"
#include "tilemap.h"

#define PLAYER_MAX_GHOSTS 8
#define PLAYER_GHOST_SPAWN_TIME 0.02f
#define PLAYER_GHOST_LIFE_TIME 0.2f

typedef struct PlayerGhost PlayerGhost;
struct PlayerGhost
{
    v2 pos;
    f32 lifeTime;
};

typedef struct Player Player;
struct Player
{
    i32 flags;
    i32 state;
    v4 rect;
    v2 pos;
    v2 vel;
    f32 moveSpeed;
    f32 dashTime;
    SpriteAnimation animation;
    PlayerGhost ghosts[PLAYER_MAX_GHOSTS];
    f32 ghostSpawnTime;
};

Player CreatePlayer(v2 pos);

void UpdatePlayer(Player *player, const Tilemap *map, f32 deltaTime);
void DrawPlayer(const Player *player);
