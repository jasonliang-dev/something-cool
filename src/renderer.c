#include "renderer.h"
#include "language.h"
#include "memory.h"
#include "texture.h"
#include <stddef.h>
#include <string.h>

#define RENDERER_MAX_QUADS 8192
#define RENDERER_MAX_TEXTURE_SLOTS 4

typedef struct Vertex Vertex;
struct Vertex
{
    v3 a_Position;
    v2 a_TexCoord;
    f32 a_TexIndex;
    v4 a_Color;
};

typedef struct Quad Quad;
struct Quad
{
    Vertex vertices[4];
};

typedef struct Renderer Renderer;
struct Renderer
{
    GLuint program;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    GLint u_Textures;
    GLint u_MVP;

    GLuint textureSlots[RENDERER_MAX_TEXTURE_SLOTS];
    i32 textureSlotCount;

    Quad quads[RENDERER_MAX_QUADS];
    i32 quadCount;
};

static Renderer g_Renderer;

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

typedef struct VertexBufferObjects VertexBufferObjects;
struct VertexBufferObjects
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

static VertexBufferObjects CreateDynamicVertexBuffer(void)
{
    VertexBufferObjects result;

    glGenVertexArrays(1, &result.vao);
    glBindVertexArray(result.vao);

    glGenBuffers(1, &result.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * RENDERER_MAX_QUADS * 4, NULL,
                 GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, a_Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, a_TexCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, a_TexIndex));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
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

    glGenBuffers(1, &result.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, RENDERER_MAX_QUADS * 6 * sizeof(u32), indices,
                 GL_STATIC_DRAW);

    return result;
}

void InitRenderer(void)
{
    const char *vert = "                                 \n\
        #version 330 core                                \n\
                                                         \n\
        layout(location = 0) in vec3 a_Position;         \n\
        layout(location = 1) in vec2 a_TexCoord;         \n\
        layout(location = 2) in float a_TexIndex;        \n\
        layout(location = 3) in vec4 a_Color;            \n\
        out vec2 v_TexCoord;                             \n\
        out float v_TexIndex;                            \n\
        out vec4 v_Color;                                \n\
        uniform mat4 u_MVP;                              \n\
                                                         \n\
        void main()                                      \n\
        {                                                \n\
            v_TexCoord = a_TexCoord;                     \n\
            v_TexIndex = a_TexIndex;                     \n\
            v_Color = a_Color;                           \n\
            gl_Position = u_MVP * vec4(a_Position, 1.0); \n\
        }                                                \n\
    ";

    const char *frag = "                                                \n\
        #version 330 core                                               \n\
                                                                        \n\
        in vec2 v_TexCoord;                                             \n\
        in float v_TexIndex;                                            \n\
        in vec4 v_Color;                                                \n\
        out vec4 f_Color;                                               \n\
        uniform sampler2D u_Textures[4];                                \n\
                                                                        \n\
        void main()                                                     \n\
        {                                                               \n\
            switch (int(v_TexIndex))                                    \n\
            {                                                           \n\
            case 0:                                                     \n\
                f_Color = texture(u_Textures[0], v_TexCoord) * v_Color; \n\
                break;                                                  \n\
            case 1:                                                     \n\
                f_Color = texture(u_Textures[1], v_TexCoord) * v_Color; \n\
                break;                                                  \n\
            case 2:                                                     \n\
                f_Color = texture(u_Textures[2], v_TexCoord) * v_Color; \n\
                break;                                                  \n\
            case 3:                                                     \n\
                f_Color = texture(u_Textures[3], v_TexCoord) * v_Color; \n\
                break;                                                  \n\
            }                                                           \n\
        }                                                               \n\
    ";

    g_Renderer.program = CreateShaderProgram(vert, frag);

    VertexBufferObjects bufferObjects = CreateDynamicVertexBuffer();
    g_Renderer.vao = bufferObjects.vao;
    g_Renderer.vbo = bufferObjects.vbo;
    g_Renderer.ebo = bufferObjects.ebo;

    memset(g_Renderer.textureSlots, 0, sizeof(g_Renderer.textureSlots));
    memset(g_Renderer.quads, 0, sizeof(g_Renderer.quads));

    g_Renderer.u_Textures = glGetUniformLocation(g_Renderer.program, "u_Textures");
    g_Renderer.u_MVP = glGetUniformLocation(g_Renderer.program, "u_MVP");

    glUseProgram(0);
}

void BeginDraw(m4 mvp)
{
    glUseProgram(g_Renderer.program);

    i32 samplers[RENDERER_MAX_TEXTURE_SLOTS];
    for (i32 i = 0; i < RENDERER_MAX_TEXTURE_SLOTS; ++i)
    {
        samplers[i] = i;
    }
    glUniform1iv(g_Renderer.u_Textures, RENDERER_MAX_TEXTURE_SLOTS, samplers);

    glUniformMatrix4fv(g_Renderer.u_MVP, 1, GL_FALSE, mvp.elements);

    g_Renderer.textureSlotCount = 0;
    g_Renderer.quadCount = 0;
}

void EndDraw(void)
{
    if (g_Renderer.quadCount == 0)
    {
        return;
    }

    for (i32 i = 0; i < g_Renderer.textureSlotCount; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, g_Renderer.textureSlots[i]);
    }

    // glBufferSubData will block until the GPU is done with the previous
    // draw call. some machines will use a blit copy to avoid the stall.

    // this is bad. oh well.

    glBindBuffer(GL_ARRAY_BUFFER, g_Renderer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, g_Renderer.quadCount * sizeof(Quad),
                    g_Renderer.quads);

    glBindVertexArray(g_Renderer.vao);
    glDrawElements(GL_TRIANGLES, g_Renderer.quadCount * 6, GL_UNSIGNED_INT, NULL);
}

void DrawQuad(m4 transform, v4 texCoords, GLuint texId, v4 color)
{
    if (g_Renderer.quadCount == RENDERER_MAX_QUADS)
    {
        EndDraw();
        g_Renderer.quadCount = 0;
        g_Renderer.textureSlotCount = 0;
    }

    i32 index = -1;
    for (i32 i = 0; i < g_Renderer.textureSlotCount; ++i)
    {
        if (texId == g_Renderer.textureSlots[i])
        {
            index = i;
        }
    }

    if (index == -1)
    {
        if (g_Renderer.textureSlotCount == RENDERER_MAX_TEXTURE_SLOTS)
        {
            EndDraw();
            g_Renderer.quadCount = 0;
            g_Renderer.textureSlotCount = 0;
        }

        index = g_Renderer.textureSlotCount;
        g_Renderer.textureSlots[g_Renderer.textureSlotCount++] = texId;
    }

    Quad *quad = g_Renderer.quads + g_Renderer.quadCount++;

    quad->vertices[0].a_Position = M4xV4(transform, v4(0.0f, 1.0f, 0.0f, 1.0f)).xyz;
    quad->vertices[1].a_Position = M4xV4(transform, v4(1.0f, 0.0f, 0.0f, 1.0f)).xyz;
    quad->vertices[2].a_Position = M4xV4(transform, v4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    quad->vertices[3].a_Position = M4xV4(transform, v4(1.0f, 1.0f, 0.0f, 1.0f)).xyz;

    quad->vertices[0].a_TexCoord = v2(texCoords.x, texCoords.w);
    quad->vertices[1].a_TexCoord = v2(texCoords.z, texCoords.y);
    quad->vertices[2].a_TexCoord = v2(texCoords.x, texCoords.y);
    quad->vertices[3].a_TexCoord = v2(texCoords.z, texCoords.w);

    quad->vertices[0].a_TexIndex = (f32)index;
    quad->vertices[1].a_TexIndex = (f32)index;
    quad->vertices[2].a_TexIndex = (f32)index;
    quad->vertices[3].a_TexIndex = (f32)index;

    quad->vertices[0].a_Color = color;
    quad->vertices[1].a_Color = color;
    quad->vertices[2].a_Color = color;
    quad->vertices[3].a_Color = color;
}
