internal void GameSceneBegin(memory_arena_t *arena)
{
    game_scene_t *scene = M_ArenaPushZero(arena, sizeof(game_scene_t));
    scene->bulletPoolCount = 0;
    scene->bulletPoolMax = 1000;
    scene->bulletPool = M_ArenaPush(arena, sizeof(bullet_t) * scene->bulletPoolMax);

    scene->player.moveSpeed = 2.0f;
    scene->player.shootCooldown = 0;
}

internal void GameSceneEnd(void *memory)
{
    (void)memory;
}

internal b32 GameSceneUpdate(void *memory, scene_t *nextScene)
{
    game_scene_t *scene = memory;

    local_persist f32 angle = 0;

    v2 cursorPos =
        v2(os->mousePosition.x * app->screenScale.x, os->mousePosition.y * app->screenScale.y);

    player_t *player = &scene->player;
    player->vel = v2(0.0f, 0.0f);

    if (app->mouseDown[MouseButton_Left] && player->shootCooldown == 0)
    {
        f32 bulletSpeed = 2.0f;
        player->shootCooldown = 10;
        bullet_t *b = &scene->bulletPool[scene->bulletPoolCount++];
        b->pos = player->pos;
        b->rot = PointDirection(player->pos, cursorPos);
        b->vel.x = bulletSpeed * Cos(b->rot);
        b->vel.y = bulletSpeed * Sin(b->rot);
    }

    for (u32 i = 0; i < scene->bulletPoolCount; i++)
    {
        bullet_t *b = &scene->bulletPool[i];
        b->pos.x += b->vel.x;
        b->pos.y += b->vel.y;
    }

    if (player->shootCooldown > 0)
    {
        player->shootCooldown--;
    }

    if (app->keyDown[Key_W])
    {
        player->vel.y -= 1.0f;
    }
    if (app->keyDown[Key_S])
    {
        player->vel.y += 1.0f;
    }
    if (app->keyDown[Key_A])
    {
        player->vel.x -= 1.0f;
    }
    if (app->keyDown[Key_D])
    {
        player->vel.x += 1.0f;
    }

    player->vel = V2Normalize(player->vel);
    player->vel = V2MultiplyF32(player->vel, player->moveSpeed);

    if (player->vel.x > 0)
    {
        player->facingLeft = 0;
    }
    else if (player->vel.x < 0)
    {
        player->facingLeft = 1;
    }

    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y;

    R_DrawTilemap(app->resources.map);
    R_DrawSpriteExt(app->resources.dog, player->pos, 0,
                    v2(player->facingLeft ? -1.0f : 1.0f, 1.0f));
    R_DrawSprite(app->resources.cursor, cursorPos, 0);

    for (u32 i = 0; i < scene->bulletPoolCount; i++)
    {
        bullet_t *b = &scene->bulletPool[i];
        R_DrawSprite(app->resources.bone, b->pos, -b->rot);
    }

    if (app->keyPress[Key_Esc])
    {
        *nextScene = SceneCreate(Menu);
        return 1;
    }

    return 0;
}
