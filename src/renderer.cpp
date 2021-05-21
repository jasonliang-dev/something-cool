#include "renderer.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <sstream>
#include <stdexcept>

static GLuint CreateShaderProgram(const char *vert, const char *frag)
{
    GLuint program = glCreateProgram();

    auto CompileGLSL = [](GLuint type, const char *source)
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

    auto ShaderProgramError = [program](const char *prefixMessage)
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
    constexpr char *vert = "                             \n\
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

    constexpr char *frag = "                          \n\
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_Vertices.resize(MAX_QUADS * 4);
    m_QuadCount = 0;
}

Renderer::~Renderer(void)
{
    glDeleteProgram(m_Program);

    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Renderer::BeginDraw(Texture *atlas, m4 mvp)
{
    glUseProgram(m_Program);

    glUniform1i(glGetUniformLocation(m_Program, "u_Texture"), 0);
    atlas->Bind(0);

    glUniformMatrix4fv(glGetUniformLocation(m_Program, "u_MVP"), 1, GL_FALSE, &mvp[0][0]);

    m_QuadCount = 0;
    m_CurrentTexture = atlas;
}

void Renderer::Flush(void)
{
    if (m_QuadCount == 0)
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_QuadCount * 4 * sizeof(Renderer::Vertex),
                    m_Vertices.data());

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glDrawElements(GL_TRIANGLES, m_QuadCount * 6, GL_UNSIGNED_INT, NULL);

    m_QuadCount = 0;
}

void Renderer::DrawTexture(v2 pos, v4 rect)
{
    assert(m_CurrentTexture);

    v2 dim = v2((f32)m_CurrentTexture->GetWidth(), (f32)m_CurrentTexture->GetHeight());

    m4 transform = glm::translate(m4(1), v3(pos.x, pos.y, 0.0f));
    transform = glm::scale(transform, v3(dim, 1.0f));

    v4 uv = {
        rect.x / dim.x,
        rect.y / dim.y,
        rect.z / dim.x,
        rect.w / dim.y,
    };

    gsl::span<Quad> quads = AllocateQuads(1);

    quads[0].vertices[0] = {transform * v4(0.0f, 1.0f, 0.0f, 1.0f), v2(uv.x, uv.w)};
    quads[0].vertices[1] = {transform * v4(1.0f, 0.0f, 0.0f, 1.0f), v2(uv.z, uv.y)};
    quads[0].vertices[2] = {transform * v4(0.0f, 0.0f, 0.0f, 1.0f), v2(uv.x, uv.y)};
    quads[0].vertices[3] = {transform * v4(1.0f, 1.0f, 0.0f, 1.0f), v2(uv.z, uv.w)};
}

gsl::span<Renderer::Quad> Renderer::AllocateQuads(i32 count)
{
    if (m_QuadCount + count > MAX_QUADS)
    {
        Flush();
    }

    gsl::span<Quad> quads(reinterpret_cast<Quad *>(&m_Vertices[m_QuadCount * 4]), count);

    m_QuadCount += count;
    return quads;
}
