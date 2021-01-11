struct player_t
{
    v2 pos;
    v2 vel;
    u32 shootCooldown;
    f32 moveSpeed;
    b32 facingLeft;
};

struct bullet_t
{
    v2 pos;
    v2 vel;
    f32 rot;
};

struct game_scene_t
{
    v2 camera;
    player_t player;
    u32 bulletPoolCount;
    u32 bulletPoolMax;
    bullet_t *bulletPool;
};

internal void GameSceneBegin(memory_arena_t *arena);
internal void GameSceneEnd(void *memory);
internal b32 GameScenePixelUpdate(void *memory, scene_t *nextScene);
internal b32 GameSceneNativeUpdate(void *memory, scene_t *nextScene);
