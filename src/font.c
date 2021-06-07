#include "font.h"
#include "memory.h"
#include "renderer.h"

Font CreateFontFace(const char *file, f32 height)
{
    enum
    {
        BITMAP_WIDTH = 512,
        BITMAP_HEIGHT = 512,
    };

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

    Font result;
    result.height = height;
    result.texture.width = BITMAP_WIDTH;
    result.texture.height = BITMAP_HEIGHT;

    u8 *bitmap = ScratchAlloc(BITMAP_WIDTH * BITMAP_HEIGHT);
    stbtt_BakeFontBitmap(ttf, 0, height, bitmap, BITMAP_WIDTH, BITMAP_HEIGHT, ' ',
                         ArrayCount(result.characters), result.characters);

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
        enum
        {
            IS_OPENGL = 1,
        };

        stbtt_aligned_quad quad;
        stbtt_GetBakedQuad(font.characters, font.texture.width, font.texture.height,
                           *text - ' ', &pos.x, &pos.y, &quad, IS_OPENGL);

        v4 rect = v4(quad.x0, quad.y0, quad.x1 - quad.x0, quad.y1 - quad.y0);
        m4 transform = M4Translate(m4(1), v3(rect.x, rect.y + font.height, 0.0f));
        transform = M4Scale(transform, v3(rect.z, rect.w, 1.0f));

        v4 texCoords = v4(quad.s0, quad.t0, quad.s1, quad.t1);

        *AllocateQuads(1) = CreateQuad(transform, texCoords, color);
        text++;
    }
}
