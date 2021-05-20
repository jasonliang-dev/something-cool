#include "texture.hpp"
#include <sstream>
#include <stb_image.h>
#include <stdexcept>

Texture::Texture(const char *filePath)
{
    i32 channels;
    u8 *imageData = stbi_load(filePath, &m_Width, &m_Height, &channels, 0);
    if (!imageData)
    {
        std::stringstream ss;
        ss << "Failed to load image " << filePath;
        throw std::runtime_error(ss.str());
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
        // 2 channels?
        std::stringstream ss;
        ss << "Can't handle image with " << channels << " channels";
        throw std::runtime_error(ss.str());
    }
    }

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Width, m_Height, 0, format,
                 GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);
}

Texture::~Texture(void)
{
    glDeleteTextures(1, &m_ID);
}

void Texture::Bind(i32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

i32 Texture::GetWidth(void) const
{
    return m_Width;
}

i32 Texture::GetHeight(void) const
{
    return m_Height;
}
