#include "player.h"
#include "input.h"

Player CreatePlayer(void)
{
    Player player;

    player.flags = 0;
    player.state = PLAYER_IDLE;
    player.pos = v2(0, 0);
    player.vel = v2(0, 0);
    player.moveSpeed = 0;
    player.dashTime = 0.0f;

    player.ani[PLAYER_IDLE] = CreateSpriteAnimation(KNIGHT_M_IDLE_ANIM);
    player.ani[PLAYER_RUN] = CreateSpriteAnimation(KNIGHT_M_RUN_ANIM);
    player.ani[PLAYER_DASH] = CreateSpriteAnimation(KNIGHT_M_RUN_ANIM);
    player.ani[PLAYER_DASH].msPerFrame = 50;

    return player;
}

void UpdatePlayer(Player *player, f32 deltaTime)
{
    switch (player->state)
    {
    case PLAYER_IDLE:
        if (KeyDown(GLFW_KEY_D) || KeyDown(GLFW_KEY_A) || KeyDown(GLFW_KEY_S) ||
            KeyDown(GLFW_KEY_W))
        {
            ResetSpriteAnimation(&player->ani[PLAYER_RUN]);
            player->moveSpeed = 80;
            player->state = PLAYER_RUN;
        }
        break;
    case PLAYER_RUN:
        player->vel.x = (f32)KeyDown(GLFW_KEY_D) - KeyDown(GLFW_KEY_A);
        player->vel.y = (f32)KeyDown(GLFW_KEY_S) - KeyDown(GLFW_KEY_W);
        player->vel = V2Normalize(player->vel);

        if (player->vel.x == 0.0f && player->vel.y == 0.0f)
        {
            ResetSpriteAnimation(&player->ani[PLAYER_IDLE]);
            player->state = PLAYER_IDLE;
            break;
        }

        if (KeyPressed(GLFW_KEY_SPACE))
        {
            ResetSpriteAnimation(&player->ani[PLAYER_DASH]);
            player->moveSpeed = 180;
            player->state = PLAYER_DASH;
            player->dashTime = 0.2f;
            break;
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

        break;
    case PLAYER_DASH:
        player->dashTime -= deltaTime;

        if (player->dashTime <= 0.0f)
        {
            ResetSpriteAnimation(&player->ani[PLAYER_IDLE]);
            player->state = PLAYER_IDLE;
        }

        player->pos.x += player->vel.x * player->moveSpeed * deltaTime;
        player->pos.y += player->vel.y * player->moveSpeed * deltaTime;

        break;
    default:
        break;
    }

    UpdateSpriteAnimation(&player->ani[player->state], deltaTime);
}

void DrawPlayer(const Player *player)
{
    v4 color = player->state == PLAYER_DASH ? v4(0.5f, 0.5f, 1, 1) : v4(1, 1, 1, 1);

    DrawSpriteAnimationExt(&player->ani[player->state], player->pos,
                           v2(player->flags & PLAYER_FACING_LEFT ? -1.0f : 1.0f, 1),
                           color);
}
