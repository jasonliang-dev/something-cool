#pragma once

#include "language.hpp"
#include "geometry.hpp"
#include "span.hpp"
#include "texture.hpp"
#include <glad/gl.h>
#include <memory>
#include <vector>

struct Renderer
{
    struct Vertex
    {
        v3 a_Position;
        v2 a_TexCoord;
    };

    union Quad
    {
        struct
        {
            Vertex bottomLeft;  // 0, 1
            Vertex topRight;    // 1, 0
            Vertex topLeft;     // 0, 0
            Vertex bottomRight; // 1, 1
        };

        Vertex vertices[4];

        Quad(void) = default;
        Quad(m4 transform, v4 texCoords);
    };

    Renderer(void);
    ~Renderer(void);

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void BeginDraw(std::shared_ptr<Texture> atlas, m4 mvp);
    void EndDraw(void);

    // write to returned buffer to draw stuff. must be in between BeginDraw/EndDraw
    [[nodiscard]] Span<Quad> AllocateQuads(i32 count);

    // ultimately calls AllocateQuads
    void DrawTexture(v2 pos);          // draw entire texture
    void DrawTexture(v2 pos, v4 rect); // draw part of texture

    static const u32 MAX_QUADS = 20000;

    GLuint m_Program;

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    std::vector<Quad> m_Quads;
    i32 m_QuadCount;

    std::shared_ptr<Texture> m_CurrentAtlas;
};
