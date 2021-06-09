#pragma once

#include "language.h"
#include "texture.h"
#include <glad/gl.h>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

#define FONT_CHARACTER_COUNT 96

typedef struct Font Font;
struct Font
{
    Texture texture;
    f32 height;
    stbtt_packedchar characters[FONT_CHARACTER_COUNT];
};

Font CreateFontFace(const char *file, f32 height);
void DrawFont(const char *text, Font font, v2 pos, v4 color);
