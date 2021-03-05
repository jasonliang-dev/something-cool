internal void CreateAllAssets()
{
#define X(name, file, width, msPerFrame)                                                           \
    CreateSpriteAnimation(&app->##name, file, width, msPerFrame);
    SPRITE_ANIMATION_ASSETS
#undef X

#define X(name, atlas, data) CreateTilemap(&app->##name, atlas, data);
    TILEMAP_ASSETS
#undef X

#define X(name, file) CreateSoundFromWAV(&app->##name, file);
    SOUND_ASSETS
#undef X
}

internal void DestroyAllAssets()
{
#define X(name, file, width, msPerFrame) DestroySpriteAnimation(&app->##name);
    SPRITE_ANIMATION_ASSETS
#undef X

#define X(name, atlas, data) DestroyTilemap(&app->##name);
    TILEMAP_ASSETS
#undef X

#define X(name, file) SDL_FreeWAV(app->##name##.buffer);
    SOUND_ASSETS
#undef X
}
