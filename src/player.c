#include "player.h"
#include "input.h"
#include "net.h"
#include <assert.h>
#include <math.h>
#include <string.h>

// state
enum PlayerState
{
    PLAYER_IDLE,
    PLAYER_RUN,
    PLAYER_DASH,
    PLAYER_STATE_MAX,
};

// flags
enum PlayerFlags
{
    PLAYER_FACING_LEFT = 1 << 0,
};

typedef struct UpdateContext UpdateContext;
struct UpdateContext
{
    Player *player;
    const Tilemap *map;
    f32 deltaTime;
};

typedef struct PlayerAppearance PlayerAppearance;
struct PlayerAppearance
{
    i32 idle;
    i32 run;
};

static PlayerAppearance g_PlayerAppearances[] = {
    [PLAYER_ELF] =
        {
            .idle = ELF_M_IDLE_ANIM,
            .run = ELF_M_RUN_ANIM,
        },
    [PLAYER_KNIGHT] =
        {
            .idle = KNIGHT_M_IDLE_ANIM,
            .run = KNIGHT_M_RUN_ANIM,
        },
    [PLAYER_WIZZARD] =
        {
            .idle = WIZZARD_M_IDLE_ANIM,
            .run = WIZZARD_M_RUN_ANIM,
        },
    [PLAYER_LIZARD] =
        {
            .idle = LIZARD_M_IDLE_ANIM,
            .run = LIZARD_M_RUN_ANIM,
        },
};

static void EnterIdle(Player *player)
{
    player->animation =
        CreateSpriteAnimation(g_PlayerAppearances[player->appearanceType].idle, 150);
}

static i32 UpdateIdle(UpdateContext context)
{
    Player *player = context.player;

    player->vel.x = (f32)KeyDown(GLFW_KEY_D) - KeyDown(GLFW_KEY_A);
    player->vel.y = (f32)KeyDown(GLFW_KEY_S) - KeyDown(GLFW_KEY_W);
    player->vel = V2Normalize(player->vel);

    if (player->vel.x != 0.0f || player->vel.y != 0.0f)
    {
        return PLAYER_RUN;
    }

    if (KeyPressed(GLFW_KEY_SPACE))
    {
        player->vel = v2((player->flags & PLAYER_FACING_LEFT) ? -1.0f : 1.0f, 0);
        return PLAYER_DASH;
    }

    return PLAYER_IDLE;
}

static void EnterRun(Player *player)
{
    player->animation =
        CreateSpriteAnimation(g_PlayerAppearances[player->appearanceType].run, 100);
    player->moveSpeed = 80;
}

static i32 UpdateRun(UpdateContext context)
{
    Player *player = context.player;
    f32 deltaTime = context.deltaTime;

    player->vel.x = (f32)KeyDown(GLFW_KEY_D) - KeyDown(GLFW_KEY_A);
    player->vel.y = (f32)KeyDown(GLFW_KEY_S) - KeyDown(GLFW_KEY_W);
    player->vel = V2Normalize(player->vel);

    if (player->vel.x == 0.0f && player->vel.y == 0.0f)
    {
        return PLAYER_IDLE;
    }

    if (player->vel.x < 0.0f)
    {
        player->flags |= PLAYER_FACING_LEFT;
    }
    else if (player->vel.x > 0.0f)
    {
        player->flags &= ~PLAYER_FACING_LEFT;
    }

    if (MouseClicked(GLFW_MOUSE_BUTTON_RIGHT))
    {
        return PLAYER_DASH;
    }

    TilemapMovement movement =
        MoveWithTilemap(context.map, player->pos,
                        v2(player->vel.x * player->moveSpeed * deltaTime,
                           player->vel.y * player->moveSpeed * deltaTime),
                        player->rect);

    player->pos = movement.pos;
    player->vel = movement.vel;

    return PLAYER_RUN;
}

static void EnterDash(Player *player)
{
    player->vel = V2Normalize(player->vel);

    player->animation =
        CreateSpriteAnimation(g_PlayerAppearances[player->appearanceType].run, 50);
    player->moveSpeed = 300;
    player->dashTime = 0.15f;
    player->ghostSpawnTime = PLAYER_GHOST_SPAWN_TIME;
}

static i32 UpdateDash(UpdateContext context)
{
    Player *player = context.player;
    f32 deltaTime = context.deltaTime;

    player->dashTime -= deltaTime;

    if (player->dashTime <= 0.0f)
    {
        return PLAYER_IDLE;
    }

    player->ghostSpawnTime -= deltaTime;

    if (player->ghostSpawnTime <= 0.0f)
    {
        player->ghostSpawnTime = PLAYER_GHOST_SPAWN_TIME;

        for (i32 i = 0; i < PLAYER_MAX_GHOSTS; ++i)
        {
            if (player->ghosts[i].lifeTime <= 0.0f)
            {
                player->ghosts[i].pos = player->pos;
                player->ghosts[i].lifeTime = PLAYER_GHOST_LIFE_TIME;
                break;
            }
        }
    }

    TilemapMovement movement =
        MoveWithTilemap(context.map, player->pos,
                        v2(player->vel.x * player->moveSpeed * deltaTime,
                           player->vel.y * player->moveSpeed * deltaTime),
                        player->rect);

    player->pos = movement.pos;

    return PLAYER_DASH;
}

typedef struct StateVTable StateVTable;
struct StateVTable
{
    void (*EnterState)(Player *player);
    i32 (*Update)(UpdateContext context);
};

static StateVTable g_States[PLAYER_STATE_MAX] = {
    [PLAYER_IDLE] = {EnterIdle, UpdateIdle},
    [PLAYER_RUN] = {EnterRun, UpdateRun},
    [PLAYER_DASH] = {EnterDash, UpdateDash},
};

Player CreatePlayer(v2 pos)
{
    Player player = {
        .flags = 0,
        .state = PLAYER_IDLE,
        .pos = pos,
        .vel = v2(0, 0),
        .moveSpeed = 0,
        .dashTime = 0.0f,
        .appearanceType = PLAYER_KNIGHT,
    };

    memset(player.ghosts, 0, sizeof(player.ghosts));

    g_States[player.state].EnterState(&player);

    player.rect = v4(-player.animation.rect.z / 2, player.animation.rect.w * 0.4f,
                     player.animation.rect.z, player.animation.rect.w * 0.2f);

    return player;
}

void UpdatePlayer(Player *player, const Tilemap *map, f32 deltaTime)
{
    for (i32 i = 0; i < PLAYER_MAX_GHOSTS; ++i)
    {
        player->ghosts[i].lifeTime -= deltaTime;
    }

    UpdateContext context;
    context.player = player;
    context.map = map;
    context.deltaTime = deltaTime;

    i32 nextState = g_States[player->state].Update(context);
    if (nextState != player->state)
    {
        player->state = nextState;
        g_States[player->state].EnterState(player);
    }

    UpdateSpriteAnimation(&player->animation, deltaTime);
    ClientSend((NetMessage){.type = TO_SERVER_PLAYER_INFO,
                            .playerInfo = {
                                .state = player->state,
                                .pos = player->pos,
                                .vel = player->vel,
                            }});
}

void DrawPlayer(const Player *player)
{
    v2 scale = v2(player->flags & PLAYER_FACING_LEFT ? -1.0f : 1.0f, 1);

    for (i32 i = 0; i < PLAYER_MAX_GHOSTS; ++i)
    {
        if (player->ghosts[i].lifeTime > 0.0f)
        {
            DrawSpriteAnimationExt(
                &player->animation, player->ghosts[i].pos, scale,
                v4(0.5f, 0.5f, 1,
                   (player->ghosts[i].lifeTime / PLAYER_GHOST_LIFE_TIME) * 0.9f));
        }
    }

    switch (player->state)
    {
    case PLAYER_DASH:
        DrawSpriteAnimationExt(&player->animation, player->pos, scale,
                               v4(0.8f, 0.8f, 1, 1));
        break;
    default:
        DrawSpriteAnimationExt(&player->animation, player->pos, scale, v4(1, 1, 1, 1));
    }
}
