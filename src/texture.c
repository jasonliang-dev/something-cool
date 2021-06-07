#include "texture.h"
#include "renderer.h"
#include <stb_image.h>

Texture CreateTexture(const char *filePath)
{
    Texture result;

    i32 channels;
    u8 *imageData = stbi_load(filePath, &result.width, &result.height, &channels, 0);
    if (!imageData)
    {
        Fatal("Failed to load image: %s", filePath);
    }

    GLenum format = 0;
    switch (channels)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default: {
        Fatal("Can't handle image with %d channels", channels);
    }
    }

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, result.width, result.height, 0, format,
                 GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    return result;
}

void DrawTexture(v2 pos)
{
    Texture atlas = g_Renderer.currentAtlas;
    DrawTextureRect(pos, v4(0.0f, 0.0f, (f32)atlas.width, (f32)atlas.height));
}

void DrawTextureRect(v2 pos, v4 rect)
{
    DrawTextureRectExt(pos, rect, v2(1, 1), v4(1, 1, 1, 1));
}

void DrawTextureRectExt(v2 pos, v4 rect, v2 scale, v4 color)
{
    Texture atlas = g_Renderer.currentAtlas;

    m4 transform = M4Translate(m4(1), v3(pos.x - (rect.z * scale.x * 0.5f),
                                         pos.y - (rect.w * scale.y * 0.5f), 0.0f));
    transform = M4Scale(transform, v3(rect.z * scale.x, rect.w * scale.y, 1.0f));

    v4 texCoords = {
        rect.x / atlas.width,
        rect.y / atlas.height,
        (rect.x + rect.z) / atlas.width,
        (rect.y + rect.w) / atlas.height,
    };

    *AllocateQuads(1) = CreateQuad(transform, texCoords, color);
}
