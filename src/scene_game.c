internal void GameSceneInit(memory_arena_t *arena)
{
    game_scene_t *scene = M_ArenaPushZero(arena, sizeof(game_scene_t));
    scene->bulletPoolCount = 0;
    scene->bulletPoolMax = 1000;
    scene->bulletPool = M_ArenaPush(arena, sizeof(bullet_t) * scene->bulletPoolMax);

    scene->player.moveSpeed = 2.0f;
    scene->player.shootCooldown = 0;
}

internal void GameSceneDestroy(game_scene_t *scene)
{
    (void)scene;
}

internal b32 GameSceneUpdate(game_scene_t *scene, scene_t *nextScene)
{
    (void)nextScene;

    local_persist f32 angle = 0;

    player_t *player = &scene->player;
    player->vel = v2(0.0f, 0.0f);

    if (app.mouseDown[MouseButton_Left] && player->shootCooldown == 0)
    {
        f32 bulletSpeed = 6.0f;
        player->shootCooldown = 10;
        bullet_t *b = &scene->bulletPool[scene->bulletPoolCount++];
        v2 cursor = v2(os->mousePosition.x / app.scale, os->mousePosition.y / app.scale);
        b->pos = player->pos;
        b->rot = PointDirection(player->pos, cursor);
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

    if (app.keyDown[Key_W])
    {
        player->vel.y -= 1.0f;
    }
    if (app.keyDown[Key_S])
    {
        player->vel.y += 1.0f;
    }
    if (app.keyDown[Key_A])
    {
        player->vel.x -= 1.0f;
    }
    if (app.keyDown[Key_D])
    {
        player->vel.x += 1.0f;
    }

    player->vel = V2Normalize(player->vel);
    player->vel = V2MultiplyF32(player->vel, player->moveSpeed);

    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y;

    R_DrawTilemap(app.mapShader, app.vao, app.resources.map);
    R_DrawSprite(app.spriteShader, app.vao, app.resources.dog, player->pos, 0);
    R_DrawSprite(app.spriteShader, app.vao, app.resources.cursor,
                 v2(os->mousePosition.x / app.scale, os->mousePosition.y / app.scale), 0);

    for (u32 i = 0; i < scene->bulletPoolCount; i++)
    {
        bullet_t *b = &scene->bulletPool[i];
        R_DrawSprite(app.spriteShader, app.vao, app.resources.bone, b->pos, -b->rot);
    }

    if (app.keyPress[Key_Esc])
    {
        *nextScene = Scene_CreateFrom(Menu);
        return 1;
    }

    return 0;
}
