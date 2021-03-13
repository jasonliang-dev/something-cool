#define MAX_QUADS 20000
#define MAX_VERTICES (MAX_QUADS * 4)
#define MAX_INDICES (MAX_QUADS * 6)

typedef struct Texture Texture;
struct Texture
{
    GLuint id;
    i32 width;
    i32 height;
};

typedef struct TextureVertex TextureVertex;
struct TextureVertex
{
    v2 position;
    v2 texCoord;
    f32 texIndex;
};

typedef struct Tilemap Tilemap;
struct Tilemap
{
    Texture atlas;
    i32 tileWidth; // height is the same
    i32 width;
    i32 height;
    v2 *vertexPositions;
    v2 *texCoords;
};

typedef struct SpriteAnimation SpriteAnimation;
struct SpriteAnimation
{
    Texture atlas;
    f32 tElapsed;
    i32 msPerFrame;
    i32 totalFrames;
    v2 *texCoords;
};

typedef struct Font Font;
struct Font
{
    Texture atlas;
    f32 size;
    stbtt_bakedchar charData[95];
};

typedef struct Renderer Renderer;
struct Renderer
{
    i32 maxTextureUnits;

    i32 textureCount;
    GLuint *textureIDs;

    i32 quadCount;
    TextureVertex *vertices;

    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLuint program;
    GLint u_View;
    v2 camera;
};
