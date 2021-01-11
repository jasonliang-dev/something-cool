enum texture_unit_t
{
    TEXTURE_UNIT_NULL,
    TEXTURE_UNIT_SPRITE,
    TEXTURE_UNIT_TILEMAP,
    TEXTURE_UNIT_FONT,
};

struct texture_t
{
    u32 width;
    u32 height;
    u32 textureID;
};

struct tilemap_t
{
    u32 cols;
    u32 rows;
    texture_t atlas;
    u32 tileSize;
};

struct font_t
{
    stbtt_bakedchar bakedCharData[95]; // ASCII (126 '~') - (32 ' ') = 94
    u32 textureID;
};

struct shaders_t
{
    u32 quad;
    u32 sprite;
    u32 tilemap;
    u32 font;
};

struct renderer_t
{
    u32 quadVAO;
    u32 pixelFBO;
    v2 screenScale;
    shaders_t shaders;
};
