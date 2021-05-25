#include "renderer.hpp"
#include <cassert>
#include <sstream>
#include <stdexcept>

Renderer::Quad::Quad(m4 transform, v4 texCoords)
{
    vertices[0].a_Position = (transform * v4{0.0f, 1.0f, 0.0f, 1.0f}).xyz;
    vertices[1].a_Position = (transform * v4{1.0f, 0.0f, 0.0f, 1.0f}).xyz;
    vertices[2].a_Position = (transform * v4{0.0f, 0.0f, 0.0f, 1.0f}).xyz;
    vertices[3].a_Position = (transform * v4{1.0f, 1.0f, 0.0f, 1.0f}).xyz;

    vertices[0].a_TexCoord = v2{texCoords.x, texCoords.w};
    vertices[1].a_TexCoord = v2{texCoords.z, texCoords.y};
    vertices[2].a_TexCoord = v2{texCoords.x, texCoords.y};
    vertices[3].a_TexCoord = v2{texCoords.z, texCoords.w};
}

static GLuint CreateShaderProgram(const char *vert, const char *frag)
{
    GLuint program = glCreateProgram();

    auto CompileGLSL = [](GLuint type, const char *source) -> GLuint
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

            std::vector<char> message(length);
            glGetShaderInfoLog(shader, length, NULL, message.data());

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

            std::ostringstream ss;
            ss << "Cannot compile " << typeStr << " shader: " << message.data() << "\n"
               << source;

            throw std::runtime_error(ss.str());
        }

        return shader;
    };

    GLuint vertexShader = CompileGLSL(GL_VERTEX_SHADER, vert);
    GLuint fragmentShader = CompileGLSL(GL_FRAGMENT_SHADER, frag);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    auto ShaderProgramError = [program](const char *prefixMessage) -> void
    {
        i32 length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> message(length);
        glGetProgramInfoLog(program, length, NULL, message.data());

        std::ostringstream ss;
        ss << prefixMessage << ": " << message.data() << "\n";
        throw std::runtime_error(ss.str());
    };

    b32 linked;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        ShaderProgramError("Cannot link program");
    }

    b32 valid;
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);

    if (!valid)
    {
        ShaderProgramError("Shader validation failed");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

Renderer::Renderer(void)
{
    const char *vert = "                                 \n\
        #version 330 core                                \n\
                                                         \n\
        layout(location = 0) in vec3 a_Position;         \n\
        layout(location = 1) in vec2 a_TexCoord;         \n\
        out vec2 v_TexCoord;                             \n\
        uniform mat4 u_MVP;                              \n\
                                                         \n\
        void main()                                      \n\
        {                                                \n\
            v_TexCoord = a_TexCoord;                     \n\
            gl_Position = u_MVP * vec4(a_Position, 1.0); \n\
        }                                                \n\
    ";

    const char *frag = "                              \n\
        #version 330 core                             \n\
                                                      \n\
        in vec2 v_TexCoord;                           \n\
        out vec4 f_Color;                             \n\
        uniform sampler2D u_Texture;                  \n\
                                                      \n\
        void main()                                   \n\
        {                                             \n\
            f_Color = texture(u_Texture, v_TexCoord); \n\
        }                                             \n\
    ";

    m_Program = CreateShaderProgram(vert, frag);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_QUADS * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, a_Position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, a_TexCoord)));

    std::vector<u32> indices(MAX_QUADS * 6);
    for (i32 i = 0, stride = 0; i < MAX_QUADS * 6; i += 6, stride += 4)
    {
        indices[i + 0] = stride + 0; // 0, 1
        indices[i + 1] = stride + 1; // 1, 0
        indices[i + 2] = stride + 2; // 0, 0

        indices[i + 3] = stride + 0; // 0, 1
        indices[i + 4] = stride + 3; // 1, 1
        indices[i + 5] = stride + 1; // 1, 0
    }

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_QUADS * 6 * sizeof(u32), indices.data(),
                 GL_STATIC_DRAW);

    m_Quads.resize(MAX_QUADS);
    m_QuadCount = 0;

    m_CurrentAtlas = nullptr;
}

Renderer::~Renderer(void)
{
    glDeleteProgram(m_Program);

    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Renderer::BeginDraw(std::shared_ptr<Texture> atlas, m4 mvp)
{
    glUseProgram(m_Program);

    glUniform1i(glGetUniformLocation(m_Program, "u_Texture"), 0);
    atlas->Bind(0);

    glUniformMatrix4fv(glGetUniformLocation(m_Program, "u_MVP"), 1, GL_FALSE, &mvp[0][0]);

    m_QuadCount = 0;
    m_CurrentAtlas = atlas;
}

void Renderer::EndDraw(void)
{
    if (m_QuadCount == 0)
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_QuadCount * sizeof(Quad), m_Quads.data());

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_QuadCount * 6, GL_UNSIGNED_INT, NULL);
}

[[nodiscard]] Span<Renderer::Quad> Renderer::AllocateQuads(i32 count)
{
    if (m_QuadCount + count > MAX_QUADS)
    {
        EndDraw();
        m_QuadCount = 0;
    }

    Span<Quad> quads(&m_Quads[m_QuadCount], count);
    m_QuadCount += count;
    return quads;
}

void Renderer::DrawTexture(v2 pos)
{
    assert(m_CurrentAtlas);
    v2 dim = m_CurrentAtlas->GetDim();
    DrawTexture(pos, v4{0.0f, 0.0f, dim.x, dim.y});
}

void Renderer::DrawTexture(v2 pos, v4 rect)
{
    assert(m_CurrentAtlas);
    v2 dim = m_CurrentAtlas->GetDim();

    m4 transform =
        m4(1).Translate(v3{pos.x, pos.y, 0.0f}).Scale(v3{rect.z, rect.w, 1.0f});

    v4 texCoords = {
        rect.x / dim.x,
        rect.y / dim.y,
        (rect.x + rect.z) / dim.x,
        (rect.y + rect.w) / dim.y,
    };

    Span<Quad> quads = AllocateQuads(1);
    quads[0] = Quad(transform, texCoords);
}
