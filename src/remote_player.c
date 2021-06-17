#include "remote_player.h"

enum RemotePlayerFlags
{
    REMOTE_PLAYER_FACING_LEFT = 1 << 0,
};

RemotePlayer CreateRemotePlayer(u32 id)
{
    RemotePlayer result = {0};
    result.id = id;
    result.flags = 0;
    result.state = NET_PLAYER_IDLE;
    result.animation = CreateSpriteAnimation(ELF_M_IDLE_ANIM);
    result.pos = v2(0, 0);
    return result;
}

void UpdateRemotePlayer(RemotePlayer *player, NetPlayerInfo info)
{
    if (player->state != info.state)
    {
        player->state = info.state;
        switch (player->state)
        {
        case NET_PLAYER_IDLE:
            player->animation = CreateSpriteAnimation(ELF_M_IDLE_ANIM);
            break;
        case NET_PLAYER_RUN:
            player->animation = CreateSpriteAnimation(ELF_M_RUN_ANIM);
            break;
        case NET_PLAYER_DASH:
            player->animation = CreateSpriteAnimation(ELF_M_RUN_ANIM);
            player->animation.msPerFrame = 50;
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
}

void DrawRemotePlayer(const RemotePlayer *player)
{
    v2 scale = v2(player->flags & REMOTE_PLAYER_FACING_LEFT ? -1.0f : 1.0f, 1);

    DrawSpriteAnimationExt(&player->animation, player->pos, scale, v4(1, 1, 1, 1));
}
