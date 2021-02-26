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
};

struct Renderer
{
    i32 maxTextureUnits;

    i32 quadCount;
    TextureVertex *vertices;

    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLuint program;
    GLint u_View;
};
