#pragma once

#include "language.h"
#include "geometry.h"
#include "gl.h"

typedef struct Texture Texture;
struct Texture
{
    GLuint id;
    union
    {
        struct
        {
            i32 width;
            i32 height;
        };

        iv2 resolution;
    };
};

Texture TextureCreate(const char *imagePath);
void TextureBind(Texture texture, i32 unit);
v3 TextureV3(Texture texture);
