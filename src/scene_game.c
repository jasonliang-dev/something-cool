internal void GameSceneBegin(memory_arena_t *arena)
{
    game_scene_t *scene = M_ArenaPushZero(arena, sizeof(game_scene_t));
    scene->bulletPoolCount = 0;
    scene->bulletPoolMax = 1000;
    scene->bulletPool = M_ArenaPush(arena, sizeof(bullet_t) * scene->bulletPoolMax);

    scene->player.moveSpeed = 2.0f;
    scene->player.shootCooldown = 0;
    scene->player.pos.x = 120.0f;
    scene->player.pos.y = 240.0f;

    OS_ShowCursor(0);
}

internal void GameSceneEnd(void *memory)
{
    (void)memory;
}

internal b32 GameSceneUpdate(void *memory, scene_t *nextScene)
{
    game_scene_t *scene = memory;

    local_persist f32 angle = 0;

    v2 cursorPos = GetCursorPosition();
    v2 worldCursor = v2(cursorPos.x + scene->camera.x - (LOW_RES_SCREEN_WIDTH / 2),
                        cursorPos.y + scene->camera.y - (LOW_RES_SCREEN_HEIGHT / 2));

    player_t *player = &scene->player;
    player->vel = v2(0.0f, 0.0f);

    if (app->mouseDown[MouseButton_Left] && player->shootCooldown == 0)
    {
        f32 bulletSpeed = 4.0f;
        player->shootCooldown = 10;
        bullet_t *b = &scene->bulletPool[scene->bulletPoolCount++];
        b->pos = player->pos;
        b->rot = PointDirection(b->pos, worldCursor);
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

    v2 target = v2(0,0);
    target = V2AddV2(target, V2MultiplyF32(player->pos, 8.0f));
    target = V2AddV2(target, V2MultiplyF32(worldCursor, 1.0f));
    target = V2DivideF32(target, 9.0f);

    scene->camera.x += (target.x - scene->camera.x) * 0.25f;
    scene->camera.y += (target.y - scene->camera.y) * 0.25f;

    v2 view = v2(-scene->camera.x + (LOW_RES_SCREEN_WIDTH / 2),
                 -scene->camera.y + (LOW_RES_SCREEN_HEIGHT / 2));

    R_DrawTilemap(app->resources.map, view);
    R_DrawSpriteExt(app->resources.dog, V2AddV2(player->pos, view), 0,
                    v2(player->facingLeft ? -1.0f : 1.0f, 1.0f), v2(0.5f, 0.5f));

    for (u32 i = 0; i < scene->bulletPoolCount; i++)
    {
        bullet_t *b = &scene->bulletPool[i];
        R_DrawSprite(app->resources.bone, V2AddV2(b->pos, view), -b->rot);
    }

    R_DrawSprite(app->resources.cursor, cursorPos, 0);

    if (app->keyPress[Key_Esc])
    {
        *nextScene = SceneCreate(Menu);
        return 1;
    }

    return 0;
}
