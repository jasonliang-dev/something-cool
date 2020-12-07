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
    u32 width;
    u32 height;
    texture_t atlas;
    u32 tileSize;
};
