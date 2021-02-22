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
    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLuint program;
    GLint u_model;
};
