#include "font.h"
#include "memory.h"
#include "renderer.h"
#include <stdlib.h>
#include <string.h>

#define FONT_FIRST_CHARACTER ' '

Font CreateFontFace(const char *file, f32 height)
{
    FILE *handle = fopen(file, "rb");
    if (!handle)
    {
        Fatal("Cannot open font file");
    }

    fseek(handle, 0, SEEK_END);
    i32 fileSize = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    u8 *ttf = ScratchAlloc(fileSize);
    fread(ttf, 1, fileSize, handle);
    fclose(handle);

    enum
    {
        BITMAP_WIDTH = 512,
        BITMAP_HEIGHT = 512,

        PACK_STRIDE = 0,
        PACK_PADDING = 2,
        H_OVERSAMPLE = 1,
        V_OVERSAMPLE = 1,
    };

    Font result = {
        .height = height,
        .texture.width = BITMAP_WIDTH,
        .texture.height = BITMAP_HEIGHT,
    };

    u8 *bitmap = ScratchAlloc(BITMAP_WIDTH * BITMAP_HEIGHT);

    stbtt_pack_context spc;
    stbtt_PackBegin(&spc, bitmap, BITMAP_WIDTH, BITMAP_HEIGHT, PACK_STRIDE, PACK_PADDING,
                    NULL);
    stbtt_PackSetOversampling(&spc, H_OVERSAMPLE, V_OVERSAMPLE);

    stbtt_pack_range range = {
        .font_size = result.height,
        .first_unicode_codepoint_in_range = FONT_FIRST_CHARACTER,
        .array_of_unicode_codepoints = NULL,
        .num_chars = FONT_CHARACTER_COUNT,
    };
    range.chardata_for_range = result.characters,
    stbtt_PackFontRanges(&spc, ttf, 0, &range, 1);

    stbtt_PackEnd(&spc);

    u8 *rgba = ScratchAlloc(BITMAP_WIDTH * BITMAP_HEIGHT * 4);
    for (i32 i = 0; i < BITMAP_WIDTH * BITMAP_HEIGHT; ++i)
    {
        u8 *pixel = rgba + (i * 4);
        pixel[0] = 255;
        pixel[1] = 255;
        pixel[2] = 255;
        pixel[3] = bitmap[i]; // alpha
    }

    glGenTextures(1, &result.texture.id);
    glBindTexture(GL_TEXTURE_2D, result.texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, BITMAP_WIDTH, BITMAP_HEIGHT, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rgba);

    glBindTexture(GL_TEXTURE_2D, 0);

    return result;
}

void DrawFont(const char *text, Font font, v2 pos, v4 color)
{
    while (*text)
    {
        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad(font.characters, font.texture.width, font.texture.height,
                            *text - FONT_FIRST_CHARACTER, &pos.x, &pos.y, &quad, 0);

        v4 rect = v4(quad.x0, quad.y0, quad.x1 - quad.x0, quad.y1 - quad.y0);
        m4 transform = M4Translate(m4(1), v3(rect.x, rect.y + font.height, 0.0f));
        transform = M4Scale(transform, v3(rect.z, rect.w, 1.0f));

        v4 texCoords = v4(quad.s0, quad.t0, quad.s1, quad.t1);

        DrawQuad(transform, texCoords, font.texture.id, color);
        text++;
    }
}

f32 CalculateTextWidth(const char *text, Font font)
{
    f32 result = 0;

    while (*text)
    {
        stbtt_packedchar *packed = font.characters + *text - FONT_FIRST_CHARACTER;
        result += packed->xadvance;
        text++;
    }

    return result;
}
