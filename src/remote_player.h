#pragma once

#include "net_message.h"
#include "sprite_animation.h"
#include "tilemap.h"

typedef struct RemotePlayer RemotePlayer;
struct RemotePlayer
{
    u32 id;
    i32 flags;
    i32 state;
    v2 pos;
    SpriteAnimation animation;
};

RemotePlayer CreateRemotePlayer(u32 id);
void UpdateRemotePlayer(RemotePlayer *player, NetPlayerInfo info);
void DrawRemotePlayer(const RemotePlayer *player);
