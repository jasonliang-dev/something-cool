#pragma once

#include "geometry.h"
#include "texture.h"
#include <glad/gl.h>

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

//

typedef struct Renderer Renderer;
struct Renderer
{
    GLuint program;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    GLint u_Textures;
    GLint u_MVP;

    Texture whiteTexture;

    GLuint textureSlots[RENDERER_MAX_TEXTURE_SLOTS];
    i32 textureSlotCount;

    Quad quads[RENDERER_MAX_QUADS];
    i32 quadCount;
};

extern Renderer g_Renderer;

void InitRenderer(void);

void BeginDraw(m4 mvp);
void EndDraw(void);
void DrawQuad(m4 transform, v4 texCoords, GLuint texId, v4 color);
