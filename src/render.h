struct Texture
{
    GLuint id;
    i32 width;
    i32 height;
};

struct Renderer
{
    i32 maxTextureUnits;
    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLuint program;
};
