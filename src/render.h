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

typedef struct r_shaders_t r_shaders_t;
struct r_shaders_t {
    u32 quad;
    u32 sprite;
    u32 tilemap;
};

typedef struct r_renderer_t r_renderer_t;
struct r_renderer_t {
    v2 screenScale;
    u32 quadVAO;
    u32 screenFBO;
    r_shaders_t shaders;
};
