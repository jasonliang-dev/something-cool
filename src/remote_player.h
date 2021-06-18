#pragma once

#include "net_message.h"
#include "sprite_animation.h"
#include "tilemap.h"
#include "player_ghosts.h"

typedef struct RemotePlayer RemotePlayer;
struct RemotePlayer
{
    u32 id;
    i32 flags;
    i32 state;
    i32 appearanceType;
    v2 pos;
    SpriteAnimation animation;
    PlayerGhosts ghosts;
};

RemotePlayer CreateRemotePlayer(u32 id);
void UpdateRemotePlayer(RemotePlayer *player, NetPlayerInfo info, f32 deltaTime);
void DrawRemotePlayer(const RemotePlayer *player);
