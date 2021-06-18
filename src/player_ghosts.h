#pragma once

#include "geometry.h"
#include "sprite_animation.h"

#define MAX_PLAYER_GHOSTS 8
#define PLAYER_GHOST_SPAWN_TIME 0.03f
#define PLAYER_GHOST_LIFE_TIME 0.2f

typedef struct PlayerGhosts PlayerGhosts;
struct PlayerGhosts
{
    v2 positions[MAX_PLAYER_GHOSTS];
    f32 lifeTimes[MAX_PLAYER_GHOSTS];

    f32 spawnTime;
};

PlayerGhosts InitPlayerGhosts(void);
void UpdatePlayerGhostLifetimes(PlayerGhosts *ghosts, f32 deltaTime);
void MaybeSpawnPlayerGhost(PlayerGhosts *ghosts, v2 pos, f32 deltaTime);
void DrawPlayerGhosts(const PlayerGhosts *ghosts, const SpriteAnimation *animation,
                      v2 scale);
