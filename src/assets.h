// name, file location, width, ms/frame
#define SPRITE_ANIMATION_ASSETS X(boy, "data/boy-idle.png", 32, 50)

// name, atlas location, export location
#define TILEMAP_ASSETS X(map, "data/tiles.png", "data/map.json")

// name, file location
#define SOUND_ASSETS                                                                               \
    X(wobble, "data/wobble.wav")                                                                   \
    X(coin, "data/coin.wav")

// name, file location, font size
#define FONT_ASSETS X(font, "c:/windows/fonts/times.ttf", 32)

typedef struct Assets Assets;
struct Assets
{
#define X(name, file, width, msPerFrame) SpriteAnimation name;
    SPRITE_ANIMATION_ASSETS
#undef X

#define X(name, atlas, data) Tilemap name;
    TILEMAP_ASSETS
#undef X

#define X(name, file) Sound name;
    SOUND_ASSETS
#undef X

#define X(name, file, size) Font name;
    FONT_ASSETS
#undef X
};
