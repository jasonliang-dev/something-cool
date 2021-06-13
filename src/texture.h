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
