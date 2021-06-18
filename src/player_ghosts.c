#include "player_ghosts.h"

PlayerGhosts InitPlayerGhosts(void)
{
    return (PlayerGhosts){0};
}

void UpdatePlayerGhostLifetimes(PlayerGhosts *ghosts, f32 deltaTime)
{
    for (i32 i = 0; i < MAX_PLAYER_GHOSTS; ++i)
    {
        ghosts->lifeTimes[i] -= deltaTime;
    }
}

void MaybeSpawnPlayerGhost(PlayerGhosts *ghosts, v2 pos, f32 deltaTime)
{
    ghosts->spawnTime -= deltaTime;

    if (ghosts->spawnTime > 0.0f)
    {
        return;
    }

    ghosts->spawnTime = PLAYER_GHOST_SPAWN_TIME;

    for (i32 i = 0; i < MAX_PLAYER_GHOSTS; ++i)
    {
        if (ghosts->lifeTimes[i] <= 0.0f)
        {
            ghosts->positions[i] = pos;
            ghosts->lifeTimes[i] = PLAYER_GHOST_LIFE_TIME;
            break;
        }
    }
}

void DrawPlayerGhosts(const PlayerGhosts *ghosts, const SpriteAnimation *animation,
                      v2 scale)
{
    for (i32 i = 0; i < MAX_PLAYER_GHOSTS; ++i)
    {
        if (ghosts->lifeTimes[i] > 0.0f)
        {
            DrawSpriteAnimationExt(
                animation, ghosts->positions[i], scale,
                v4(0.5f, 0.5f, 1,
                   (ghosts->lifeTimes[i] / PLAYER_GHOST_LIFE_TIME) * 0.9f));
        }
    }
}
