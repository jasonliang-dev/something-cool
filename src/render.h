typedef struct texture texture;
struct texture
{
    u32 width;
    u32 height;
    u32 textureID;
};

typedef struct tilemap tilemap;
struct tilemap
{
    u32 width;
    u32 height;
    texture atlas;
    u32 tileSize;
};
