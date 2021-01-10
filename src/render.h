typedef enum texture_unit_t texture_unit_t;
enum texture_unit_t {
    TEXTURE_UNIT_NULL,
    TEXTURE_UNIT_SPRITE,
    TEXTURE_UNIT_TILEMAP,
    TEXTURE_UNIT_FONT,
};

typedef struct texture_t texture_t;
struct texture_t
{
    u32 width;
    u32 height;
    u32 textureID;
};

typedef struct tilemap_t tilemap_t;
struct tilemap_t
{
    u32 cols;
    u32 rows;
    texture_t atlas;
    u32 tileSize;
};

typedef struct font_t font_t;
struct font_t
{
    stbtt_bakedchar bakedCharData[95]; // ASCII (126 '~') - (32 ' ') = 94
    u32 textureID;
};

typedef struct shaders_t shaders_t;
struct shaders_t
{
    u32 quad;
    u32 sprite;
    u32 tilemap;
    u32 font;
};

typedef struct renderer_t renderer_t;
struct renderer_t
{
    u32 quadVAO;
    u32 screenFBO;
    v2 screenScale;
    shaders_t shaders;
};
