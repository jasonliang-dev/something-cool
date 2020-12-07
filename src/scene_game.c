typedef struct player_t player_t;
struct player_t
{
    v2 pos;
    v2 vel;
    u32 shootCooldown;
    f32 moveSpeed;
    b32 facingRight;
};

typedef struct bullet_t bullet_t;
struct bullet_t
{
    v2 pos;
    v2 vel;
    f32 rot;
};

typedef struct game_scene game_scene;
struct game_scene
{
    player_t player;
    u32 bulletPoolCount;
    u32 bulletPoolMax;
    bullet_t *bulletPool;
};

internal void GameSceneInit(memory_arena *arena)
{
    game_scene *scene = M_ArenaPush(arena, sizeof(game_scene));
    scene->bulletPoolCount = 0;
    scene->bulletPoolMax = 1000;
    scene->bulletPool = M_ArenaPush(arena, sizeof(bullet_t) * scene->bulletPoolMax);

    scene->player.moveSpeed = 2.0f;
    scene->player.shootCooldown = 0;
}

internal void GameSceneDestroy(memory_arena *arena)
{
    (void)arena;
}

internal void GameSceneUpdate(memory_arena *arena)
{
    game_scene *scene = arena->base;

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

    if (app.keysDown[Key_W])
    {
        player->vel.y -= 1.0f;
    }
    if (app.keysDown[Key_S])
    {
        player->vel.y += 1.0f;
    }
    if (app.keysDown[Key_A])
    {
        player->vel.x -= 1.0f;
    }
    if (app.keysDown[Key_D])
    {
        player->vel.x += 1.0f;
    }

    player->vel = V2Normalize(player->vel);
    player->vel = V2MultiplyF32(player->vel, player->moveSpeed);

    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    R_DrawTilemap(app.mapShader, app.vao, app.resources.map);
    R_DrawSprite(app.spriteShader, app.vao, app.resources.dog, player->pos, 0);
    R_DrawSprite(app.spriteShader, app.vao, app.resources.cursor,
                 v2(os->mousePosition.x / app.scale - (app.resources.cursor.width / 2.0f),
                    os->mousePosition.y / app.scale - (app.resources.cursor.height / 2.0f)),
                 0);

    for (u32 i = 0; i < scene->bulletPoolCount; i++)
    {
        bullet_t *b = &scene->bulletPool[i];
        R_DrawSprite(app.spriteShader, app.vao, app.resources.bone, b->pos, -b->rot);
    }

    OS_GLSwapBuffers();
    GL_CheckForErrors();
}
