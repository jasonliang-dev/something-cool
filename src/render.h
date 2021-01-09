typedef enum r_texture_unit_t r_texture_unit_t;
enum r_texture_unit_t {
    TEXTURE_UNIT_NULL,
    TEXTURE_UNIT_SPRITE,
    TEXTURE_UNIT_TILEMAP,
    TEXTURE_UNIT_FONT,
};

typedef struct r_texture_t r_texture_t;
struct r_texture_t
{
    u32 width;
    u32 height;
    u32 textureID;
};

typedef struct r_tilemap_t r_tilemap_t;
struct r_tilemap_t
{
    u32 cols;
    u32 rows;
    r_texture_t atlas;
    u32 tileSize;
};

typedef struct r_font_t r_font_t;
struct r_font_t
{
    stbtt_bakedchar bakedCharData[95]; // ASCII [126 '~'] - [32 ' '] = 95
    u32 textureID;
};

typedef struct r_shaders_t r_shaders_t;
struct r_shaders_t
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
    r_shaders_t shaders;
};
