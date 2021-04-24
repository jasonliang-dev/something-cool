#include "texture.h"
#include "file_io.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static u8 *ReadBMP(i32 *width, i32 *height, i32 *channels, const char *filename)
{
    enum
    {
        DATA_OFFSET = 0xA,
        WIDTH_OFFSET = 0x12,
        HEIGHT_OFFSET = 0x16,
        BITS_PER_PIXEL_OFFSET = 0x1C,
    };

    i32 fileSize;
    u8 *fileData = ReadFileData(&fileSize, filename);
    if (!fileData)
    {
        return NULL;
    }

    i32 dataOffset = *(i32 *)(fileData + DATA_OFFSET);
    *width = *(i32 *)(fileData + WIDTH_OFFSET);
    *height = *(i32 *)(fileData + HEIGHT_OFFSET);
    *channels = *(i16 *)(fileData + BITS_PER_PIXEL_OFFSET) / 8;

    // assuming no padding (already 4 bytes aligned)
    u8 *imageData = malloc((*width) * (*height) * (*channels));
    for (i32 y = 0; y < *height; ++y)
    {
        for (i32 x = 0; x < *width; ++x)
        {
            u8 *dest = imageData + (y * (*width) + x) * (*channels);
            u8 *src = fileData + dataOffset + ((*height - 1 - y) * (*width) + x) * (*channels);

            // convert from bgra to rgba
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = src[3];
        }
    }

    free(fileData);
    return imageData;
}

Texture TextureCreate(const char *filename)
{
    Texture result;

    i32 channels;
    u8 *imageData = ReadBMP(&result.width, &result.height, &channels, filename);
    if (!imageData)
    {
        fprintf(stderr, "Failed to load image %s\n", filename);
        abort();
    }

    GLenum format;
    switch (channels)
    {
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RED;
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
    free(imageData);

    return result;
}

void TextureBind(Texture texture, i32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

v3 TextureV3(Texture texture)
{
    return v3((f32)texture.width, (f32)texture.height, 0);
}
