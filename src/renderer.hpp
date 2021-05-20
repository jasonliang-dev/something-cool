#pragma once

#include "language.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <glad/gl.h>
#include <gsl/span>
#include <vector>
#include <optional>

class Renderer
{
  public:
    Renderer(void);
    ~Renderer(void);

    void BeginDraw(Texture *atlas, m4 mvp);
    void Flush(void);
    void DrawTexture(v2 pos, v2 rect);

  private:
    static const u32 MAX_QUADS = 20000;

    struct Vertex
    {
        v3 a_Position;
        v2 a_TexCoord;
    };

    union Quad
    {
        struct
        {
            Vertex bottomLeft;
            Vertex topRight;
            Vertex topLeft;
            Vertex bottomRight;
        };

        Vertex vertices[4];
    };

    GLuint m_Program;

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    std::vector<Vertex> m_Vertices;
    i32 m_QuadCount;

    Texture *m_CurrentTexture;

    gsl::span<Quad> AllocateQuads(i32 count);
};
