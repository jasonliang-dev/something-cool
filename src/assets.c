internal void CreateAllAssets()
{
#define X(name, file, width, msPerFrame)                                                           \
    CreateSpriteAnimation(&assets.name, file, width, msPerFrame);
    SPRITE_ANIMATION_ASSETS
#undef X

#define X(name, atlas, data) CreateTilemap(&assets.name, atlas, data);
    TILEMAP_ASSETS
#undef X

#define X(name, file) CreateSoundFromWAV(&assets.name, file);
    SOUND_ASSETS
#undef X
}

internal void DestroyAllAssets()
{
#define X(name, file, width, msPerFrame) DestroySpriteAnimation(&assets.name);
    SPRITE_ANIMATION_ASSETS
#undef X

#define X(name, atlas, data) DestroyTilemap(&assets.name);
    TILEMAP_ASSETS
#undef X

#define X(name, file) SDL_FreeWAV(assets.name.buffer);
    SOUND_ASSETS
#undef X
}
