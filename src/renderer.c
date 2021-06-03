#include "renderer.h"
#include "language.h"
#include "memory.h"
#include <string.h>
#include <stddef.h>

Renderer g_Renderer;

Quad CreateQuad(m4 transform, v4 texCoords, v4 color)
{
    Quad result;

    result.vertices[0].a_Position = M4xV4(transform, v4(0.0f, 1.0f, 0.0f, 1.0f)).xyz;
    result.vertices[1].a_Position = M4xV4(transform, v4(1.0f, 0.0f, 0.0f, 1.0f)).xyz;
    result.vertices[2].a_Position = M4xV4(transform, v4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    result.vertices[3].a_Position = M4xV4(transform, v4(1.0f, 1.0f, 0.0f, 1.0f)).xyz;

    result.vertices[0].a_TexCoord = v2(texCoords.x, texCoords.w);
    result.vertices[1].a_TexCoord = v2(texCoords.z, texCoords.y);
    result.vertices[2].a_TexCoord = v2(texCoords.x, texCoords.y);
    result.vertices[3].a_TexCoord = v2(texCoords.z, texCoords.w);

    result.vertices[0].a_Color = color;
    result.vertices[1].a_Color = color;
    result.vertices[2].a_Color = color;
    result.vertices[3].a_Color = color;

    return result;
}

static GLuint CompileGLSL(GLuint type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    b32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        i32 length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char *message = ScratchAlloc(length);
        glGetShaderInfoLog(shader, length, NULL, message);

        const char *typeStr;
        switch (type)
        {
        case GL_VERTEX_SHADER:
            typeStr = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            typeStr = "fragment";
            break;
        default:
            typeStr = "unknown";
        }

        Fatal("Cannot compile %s shader: %s", typeStr, message, source);
    }

    return shader;
}

static GLuint CreateShaderProgram(const char *vert, const char *frag)
{
    GLuint program = glCreateProgram();

    GLuint vertexShader = CompileGLSL(GL_VERTEX_SHADER, vert);
    GLuint fragmentShader = CompileGLSL(GL_FRAGMENT_SHADER, frag);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    b32 linked;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        i32 length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        char *message = ScratchAlloc(length);
        glGetProgramInfoLog(program, length, NULL, message);

        Fatal("Cannot link program: %s", message);
    }

    b32 valid;
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);

    if (!valid)
    {
        i32 length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        char *message = ScratchAlloc(length);
        glGetProgramInfoLog(program, length, NULL, message);

        Fatal("Shader validation failed: %s", message);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void InitRenderer(void)
{
    const char *vert = "                                 \n\
        #version 330 core                                \n\
                                                         \n\
        layout(location = 0) in vec3 a_Position;         \n\
        layout(location = 1) in vec2 a_TexCoord;         \n\
        layout(location = 2) in vec4 a_Color;            \n\
        out vec2 v_TexCoord;                             \n\
        out vec4 v_Color;                                \n\
        uniform mat4 u_MVP;                              \n\
                                                         \n\
        void main()                                      \n\
        {                                                \n\
            v_TexCoord = a_TexCoord;                     \n\
            v_Color = a_Color;                           \n\
            gl_Position = u_MVP * vec4(a_Position, 1.0); \n\
        }                                                \n\
    ";

    const char *frag = "                                        \n\
        #version 330 core                                       \n\
                                                                \n\
        in vec2 v_TexCoord;                                     \n\
        in vec4 v_Color;                                        \n\
        out vec4 f_Color;                                       \n\
        uniform sampler2D u_Texture;                            \n\
                                                                \n\
        void main()                                             \n\
        {                                                       \n\
            f_Color = texture(u_Texture, v_TexCoord) * v_Color; \n\
        }                                                       \n\
    ";

    g_Renderer.program = CreateShaderProgram(vert, frag);

    glGenVertexArrays(1, &g_Renderer.vao);
    glBindVertexArray(g_Renderer.vao);

    glGenBuffers(1, &g_Renderer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_Renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * RENDERER_MAX_QUADS * 4, NULL,
                 GL_DYNAMIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, a_Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, a_TexCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, a_Color));

    u32 indices[RENDERER_MAX_QUADS * 6];
    for (i32 i = 0, stride = 0; i < RENDERER_MAX_QUADS * 6; i += 6, stride += 4)
    {
        indices[i + 0] = stride + 0; // 0, 1
        indices[i + 1] = stride + 1; // 1, 0
        indices[i + 2] = stride + 2; // 0, 0

        indices[i + 3] = stride + 0; // 0, 1
        indices[i + 4] = stride + 3; // 1, 1
        indices[i + 5] = stride + 1; // 1, 0
    }

    glGenBuffers(1, &g_Renderer.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_Renderer.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, RENDERER_MAX_QUADS * 6 * sizeof(u32), indices,
                 GL_STATIC_DRAW);

    memset(g_Renderer.quads, 0, sizeof(g_Renderer.quads));

    glUseProgram(0);
}

void BeginDraw(Texture atlas, m4 mvp)
{
    glUseProgram(g_Renderer.program);

    // TODO store uniform loc
    glUniform1i(glGetUniformLocation(g_Renderer.program, "u_Texture"), 0);
    glBindTexture(GL_TEXTURE_2D, atlas.id);

    glUniformMatrix4fv(glGetUniformLocation(g_Renderer.program, "u_MVP"), 1, GL_FALSE,
                       mvp.elements);

    g_Renderer.quadCount = 0;
    g_Renderer.currentAtlas = atlas;
}

void EndDraw(void)
{
    if (g_Renderer.quadCount == 0)
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, g_Renderer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, g_Renderer.quadCount * sizeof(Quad),
                    g_Renderer.quads);

    glBindVertexArray(g_Renderer.vao);
    glDrawElements(GL_TRIANGLES, g_Renderer.quadCount * 6, GL_UNSIGNED_INT, NULL);
}

Quad *AllocateQuads(i32 count)
{
    if (g_Renderer.quadCount + count > RENDERER_MAX_QUADS)
    {
        EndDraw();
        g_Renderer.quadCount = 0;
    }

    Quad *begin = g_Renderer.quads + g_Renderer.quadCount;
    g_Renderer.quadCount += count;
    return begin;
}
