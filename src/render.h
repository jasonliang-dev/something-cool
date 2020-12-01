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
    u32 atlas;
    u32 dataLength;
    u32 data[1];
};
