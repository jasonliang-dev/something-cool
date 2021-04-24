#include "gfx.h"

typedef struct VertexAttribState VertexAttribState;
struct VertexAttribState
{
    GLsizei stride;
    u32 location;
    i32 lastSize;
};

static VertexAttribState g_AttribState;

VAO VAOCreate(void)
{
    VAO vao;
    glGenVertexArrays(1, &vao);
    VAOBind(vao);

    return vao;
}

void VAODestory(VAO vao)
{
    glDeleteVertexArrays(1, &vao);
}

void VAOBind(VAO vao)
{
    glBindVertexArray(vao);
}

VBO VBOCreate(GLenum target, size_t n, const void *data, GLenum usage)
{
    VBO vbo;
    vbo.target = target;
    glGenBuffers(1, &vbo.handle);

    VBOBind(vbo);
    glBufferData(vbo.target, n, data, usage);

    return vbo;
}

void VBODestroy(VBO vbo)
{
    glDeleteBuffers(1, &vbo.handle);
}

void VBOBind(VBO vbo)
{
    glBindBuffer(vbo.target, vbo.handle);
}

void VertexAttrBegin(GLsizei stride)
{
    g_AttribState.stride = stride;
    g_AttribState.location = 0;
    g_AttribState.lastSize = 0;
}

void VertexAttrPush(i32 size)
{
    glEnableVertexAttribArray(g_AttribState.location);
    glVertexAttribPointer(g_AttribState.location, size, GL_FLOAT, GL_FALSE, g_AttribState.stride,
                          (void *)(sizeof(f32) * g_AttribState.lastSize));

    g_AttribState.location++;
    g_AttribState.lastSize = size;
}
