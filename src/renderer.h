#pragma once

#include "geometry.h"
#include "texture.h"
#include <glad/gl.h>

#define RENDERER_MAX_QUADS 20000

typedef struct Vertex Vertex;
struct Vertex
{
    v3 a_Position;
    v2 a_TexCoord;
    v4 a_Color;
};

typedef struct Quad Quad;
struct Quad
{
    Vertex vertices[4];
};

Quad CreateQuad(m4 transform, v4 texCoords, v4 color);

//

typedef struct Renderer Renderer;
struct Renderer
{
    GLuint program;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    GLint u_Texture;
    GLint u_MVP;

    Quad quads[RENDERER_MAX_QUADS];
    i32 quadCount;

    Texture whiteTexture;
    Texture currentAtlas;
};

extern Renderer g_Renderer;

void InitRenderer(void);

void BeginDraw(Texture atlas, m4 mvp);
void EndDraw(void);
Quad *AllocateQuads(i32 count);
