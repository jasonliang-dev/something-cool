#include "player.h"
#include "input.h"

static void EnterIdle(Player *player)
{
    player->animation = CreateSpriteAnimation(KNIGHT_M_IDLE_ANIM);
    player->animation.msPerFrame = 150;
}

static void EnterRun(Player *player)
{
    player->animation = CreateSpriteAnimation(KNIGHT_M_RUN_ANIM);
    player->moveSpeed = 80;
}

static void EnterDash(Player *player)
{
    player->animation = CreateSpriteAnimation(KNIGHT_M_RUN_ANIM);
    player->animation.msPerFrame = 50;
    player->moveSpeed = 200;
    player->dashTime = 0.2f;
}

static PlayerState UpdateIdle(Player *player, f32 deltaTime)
{
    (void)player;
    (void)deltaTime;

    if (KeyDown(GLFW_KEY_D) || KeyDown(GLFW_KEY_A) || KeyDown(GLFW_KEY_S) ||
        KeyDown(GLFW_KEY_W))
    {
        return PLAYER_RUN;
    }

    return PLAYER_IDLE;
}

static PlayerState UpdateRun(Player *player, f32 deltaTime)
{
    player->vel.x = (f32)KeyDown(GLFW_KEY_D) - KeyDown(GLFW_KEY_A);
    player->vel.y = (f32)KeyDown(GLFW_KEY_S) - KeyDown(GLFW_KEY_W);
    player->vel = V2Normalize(player->vel);

    if (player->vel.x == 0.0f && player->vel.y == 0.0f)
    {
        return PLAYER_IDLE;
    }

    if (KeyPressed(GLFW_KEY_SPACE))
    {
        return PLAYER_DASH;
    }

    if (player->vel.x < 0.0f)
    {
        player->flags |= PLAYER_FACING_LEFT;
    }
    else if (player->vel.x > 0.0f)
    {
        player->flags &= ~PLAYER_FACING_LEFT;
    }

    player->vel.x *= player->moveSpeed * deltaTime;
    player->vel.y *= player->moveSpeed * deltaTime;

    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y;

    return PLAYER_RUN;
}

static PlayerState UpdateDash(Player *player, f32 deltaTime)
{
    player->dashTime -= deltaTime;

    if (player->dashTime <= 0.0f)
    {
        return PLAYER_IDLE;
    }

    player->pos.x += player->vel.x * player->moveSpeed * deltaTime;
    player->pos.y += player->vel.y * player->moveSpeed * deltaTime;

    return PLAYER_DASH;
}

static const void (*EnterState[PLAYER_STATE_MAX])(Player *player) = {
    [PLAYER_IDLE] = EnterIdle,
    [PLAYER_RUN] = EnterRun,
    [PLAYER_DASH] = EnterDash,
};

static const PlayerState (*Update[PLAYER_STATE_MAX])(Player *player, f32 deltaTime) = {
    [PLAYER_IDLE] = UpdateIdle,
    [PLAYER_RUN] = UpdateRun,
    [PLAYER_DASH] = UpdateDash,
};

Player CreatePlayer(void)
{
    Player player;

    player.flags = 0;
    player.state = PLAYER_IDLE;
    player.pos = v2(0, 0);
    player.vel = v2(0, 0);
    player.moveSpeed = 0;
    player.dashTime = 0.0f;

    EnterState[player.state](&player);

    return player;
}

void UpdatePlayer(Player *player, f32 deltaTime)
{
    PlayerState nextState = Update[player->state](player, deltaTime);
    if (nextState != player->state)
    {
        player->state = nextState;
        EnterState[player->state](player);
    }

    UpdateSpriteAnimation(&player->animation, deltaTime);
}

void DrawPlayer(const Player *player)
{
    v4 color = player->state == PLAYER_DASH ? v4(0.5f, 0.5f, 1, 1) : v4(1, 1, 1, 1);

    DrawSpriteAnimationExt(&player->animation, player->pos,
                           v2(player->flags & PLAYER_FACING_LEFT ? -1.0f : 1.0f, 1),
                           color);
}
