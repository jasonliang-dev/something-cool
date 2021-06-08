#pragma once

#include "geometry.h"
#include "language.h"
#include <glad/gl.h>

typedef struct Texture Texture;
struct Texture
{
    GLuint id;
    i32 width;
    i32 height;
};

Texture CreateTexture(const char *filePath);

// drawn with origin in center
// because scaling (/shrug)
void DrawTexture(v2 pos);
void DrawTextureRect(v2 pos, v4 rect);
void DrawTextureRectExt(v2 pos, v4 rect, v2 scale, v4 color);
