#include "remote_player.h"
#include "player_appearance.h"

enum RemotePlayerFlags
{
    REMOTE_PLAYER_FACING_LEFT = 1 << 0,
};

RemotePlayer CreateRemotePlayer(u32 id)
{
    RemotePlayer player = {0};
    player.id = id;
    player.flags = 0;
    player.appearanceType = PLAYER_APPEARANCE_ELF_M;
    player.state = NET_PLAYER_IDLE;
    player.animation = PlayerAnimateIdle(player.appearanceType);
    player.pos = v2(0, 0);
    player.ghosts = InitPlayerGhosts();
    return player;
}

void UpdateRemotePlayer(RemotePlayer *player, NetPlayerInfo info, f32 deltaTime)
{
    if (player->state != info.state)
    {
        player->state = info.state;
        switch (player->state)
        {
        case NET_PLAYER_IDLE:
            player->animation = PlayerAnimateIdle(player->appearanceType);
            break;
        case NET_PLAYER_RUN:
            player->animation = PlayerAnimateRun(player->appearanceType);
            break;
        case NET_PLAYER_DASH:
            player->animation = PlayerAnimateDash(player->appearanceType);
            break;
        }
    }

    player->pos = info.pos;

    if (info.vel.x < 0)
    {
        player->flags |= REMOTE_PLAYER_FACING_LEFT;
    }
    else if (info.vel.x > 0)
    {
        player->flags &= ~REMOTE_PLAYER_FACING_LEFT;
    }

    UpdatePlayerGhostLifetimes(&player->ghosts, deltaTime);
    if (player->state == NET_PLAYER_DASH)
    {
        MaybeSpawnPlayerGhost(&player->ghosts, player->pos, deltaTime);
    }

    UpdateSpriteAnimation(&player->animation, deltaTime);
}

void DrawRemotePlayer(const RemotePlayer *player)
{
    v2 scale = v2(player->flags & REMOTE_PLAYER_FACING_LEFT ? -1.0f : 1.0f, 1);

    DrawPlayerGhosts(&player->ghosts, &player->animation, scale);
    DrawSpriteAnimationExt(&player->animation, player->pos, scale, v4(1, 1, 1, 1));
}
