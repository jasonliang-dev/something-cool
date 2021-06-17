#pragma once

#include "geometry.h"

#define MAX_PLAYER_LIST_COUNT 32

enum NetMessageType
{
    TO_SERVER_PLAYER_ID,
    TO_CLIENT_PLAYER_ID,

    TO_SERVER_PLAYER_LIST,
    TO_CLIENT_PLAYER_LIST,

    TO_SERVER_PLAYER_INFO,
    TO_CLIENT_PLAYER_INFO,

    TO_CLIENT_PLAYER_JOIN,
    TO_CLIENT_PLAYER_LEAVE,
};

// .playerInfo.state
enum NetPlayerState
{
    NET_PLAYER_IDLE,
    NET_PLAYER_RUN,
    NET_PLAYER_DASH,
    NET_PLAYER_STATE_MAX,
};

typedef struct NetPlayerInfo NetPlayerInfo;
struct NetPlayerInfo
{
    i32 state;
    v2 pos;
    v2 vel;
};

typedef struct NetMessage NetMessage;
struct NetMessage
{
    i32 type;
    u32 id;

    union
    {
        struct
        {
            u32 ids[MAX_PLAYER_LIST_COUNT];
            i32 count;
        } playerList;

        NetPlayerInfo playerInfo;
    };
};
