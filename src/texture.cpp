#include "texture.hpp"
#include <HandmadeMath.h>
#include <sstream>
#include <stb_image.h>
#include <stdexcept>
#include <utility>

Texture::Texture(const char *filePath)
{
    i32 channels;
    u8 *imageData = stbi_load(filePath, &m_Width, &m_Height, &channels, 0);
    if (!imageData)
    {
        std::ostringstream ss;
        ss << "Failed to load image: " << filePath;
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
        std::ostringstream ss;
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

Texture::Texture(Texture &&other) noexcept
    : m_ID(std::exchange(other.m_ID, 0)), m_Width(std::exchange(other.m_Width, 0)),
      m_Height(std::exchange(other.m_Height, 0))
{
}

Texture &Texture::operator=(Texture &&other) noexcept
{
    glDeleteTextures(1, &m_ID);

    m_ID = std::exchange(other.m_ID, 0);
    m_Width = std::exchange(other.m_Width, 0);
    m_Height = std::exchange(other.m_Height, 0);

    return *this;
}

void Texture::Bind(i32 unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

v2 Texture::GetDim(void) const
{
    return v2{(f32)m_Width, (f32)m_Height};
}
