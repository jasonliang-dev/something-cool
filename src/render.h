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
    v3 position;
    v4 color;
    v2 texCoord;
    f32 texIndex;
    f32 tilingFactor;
};

struct Renderer
{
    i32 maxTextureUnits;
    v4 vertexPositions[4];
    v2 texCoords[4];

    i32 textureCount;
    GLuint *textureIDs;

    TextureVertex *currentVertex;
    TextureVertex *vertices;

    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLuint program;

    // uniform location
    GLint u_View;
};
