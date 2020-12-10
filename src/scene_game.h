typedef struct player_t player_t;
struct player_t
{
    v2 pos;
    v2 vel;
    u32 shootCooldown;
    f32 moveSpeed;
    b32 facingLeft;
};

typedef struct bullet_t bullet_t;
struct bullet_t
{
    v2 pos;
    v2 vel;
    f32 rot;
};

typedef struct game_scene_t game_scene_t;
struct game_scene_t
{
    player_t player;
    u32 bulletPoolCount;
    u32 bulletPoolMax;
    bullet_t *bulletPool;
};

internal void GameSceneBegin(memory_arena_t *arena);
internal void GameSceneEnd(void *memory);
internal b32 GameSceneUpdate(void *memory, scene_t *nextScene);
