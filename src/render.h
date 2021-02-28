const i32 MAX_QUADS = 20000;
const i32 MAX_VERTICES = MAX_QUADS * 4;
const i32 MAX_INDICES = MAX_QUADS * 6;

struct Texture
{
    GLuint id;
    i32 width;
    i32 height;
};

struct TextureVertex
{
    v2 position;
    v2 texCoord;
    f32 texIndex;
};

struct Tilemap
{
    Texture atlas;
    i32 tileWidth; // height is the same
    i32 width;
    i32 height;
    v2 *vertexPositions;
    v2 *texCoords;
};

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
};
