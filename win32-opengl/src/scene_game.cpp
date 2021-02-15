internal void GameSceneBegin(memory_arena_t *arena)
{
    game_scene_t *scene = (game_scene_t *)Memory_ArenaPushZero(arena, sizeof(game_scene_t));
    scene->bulletPoolCount = 0;
    scene->bulletPoolMax = 1000;
    scene->bulletPool =
        (bullet_t *)Memory_ArenaPush(arena, sizeof(bullet_t) * scene->bulletPoolMax);

    scene->player.moveSpeed = 2.0f;
    scene->player.shootCooldown = 0;
    scene->player.pos.x = 120.0f;
    scene->player.pos.y = 240.0f;

    OS_ShowCursor(false);
}

internal void GameSceneEnd(void *memory)
{
    (void)memory;
}

internal b32 GameScenePixelUpdate(void *memory, scene_t *nextScene)
{
    game_scene_t *scene = (game_scene_t *)memory;

    v2 cursorPos = GetCursorPosition();
    v2 worldCursor = v2(cursorPos.x + scene->camera.x - (LOW_RES_SCREEN_WIDTH / 2),
                        cursorPos.y + scene->camera.y - (LOW_RES_SCREEN_HEIGHT / 2));

    player_t *player = &scene->player;
    player->vel = v2(0.0f, 0.0f);

    if (app->mouseDown[OS_MouseButton_Left] && player->shootCooldown == 0)
    {
        f32 bulletSpeed = 4.0f;
        player->shootCooldown = 10;
        bullet_t *b = &scene->bulletPool[scene->bulletPoolCount++];
        b->pos = player->pos;
        b->rot = PointDirection(b->pos, worldCursor);
        b->vel.x = bulletSpeed * Cos(b->rot);
        b->vel.y = bulletSpeed * Sin(b->rot);

        Audio_PlaySound(&app->audio, &app->resources.sndImpact);
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

    if (app->keyDown[OS_Key_W])
    {
        player->vel.y -= 1.0f;
    }
    if (app->keyDown[OS_Key_S])
    {
        player->vel.y += 1.0f;
    }
    if (app->keyDown[OS_Key_A])
    {
        player->vel.x -= 1.0f;
    }
    if (app->keyDown[OS_Key_D])
    {
        player->vel.x += 1.0f;
    }

    player->vel = V2Normalize(player->vel);
    player->vel = player->vel * player->moveSpeed;

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

    v2 target = v2(0, 0);
    target = target + player->pos * 8.0f;
    target = target + worldCursor * 1.0f;
    target = target / 9.0f;

    scene->camera.x += (target.x - scene->camera.x) * 0.25f;
    scene->camera.y += (target.y - scene->camera.y) * 0.25f;

    v2 view = v2(-scene->camera.x + (LOW_RES_SCREEN_WIDTH / 2),
                 -scene->camera.y + (LOW_RES_SCREEN_HEIGHT / 2));

    Render_DrawTilemap(app->resources.map, view);
    Render_DrawSpriteExt(app->resources.texDog, player->pos + view, 0,
                         v2(player->facingLeft ? -1.0f : 1.0f, 1.0f), v2(0.5f, 0.5f));

    for (u32 i = 0; i < scene->bulletPoolCount; i++)
    {
        bullet_t *b = &scene->bulletPool[i];
        Render_DrawSprite(app->resources.texBone, b->pos + view, -b->rot);
    }

    Render_DrawSprite(app->resources.texCursor, cursorPos, 0);

    if (app->keyPress[OS_Key_Esc])
    {
        *nextScene = SceneCreate(Menu);
        return true;
    }

    return false;
}

internal b32 GameSceneNativeUpdate(void *memory, scene_t *nextScene)
{
    (void)memory;
    (void)nextScene;
    return false;
}
