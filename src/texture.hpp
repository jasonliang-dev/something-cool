#pragma once

#include "geometry.hpp"
#include "language.hpp"
#include <glad/gl.h>

struct Texture
{
    GLuint m_ID = 0;
    i32 m_Width = 0;
    i32 m_Height = 0;

    void Bind(i32 unit) const;
    v2 GetDim(void) const;
};

struct TextureHandle : public Texture
{
    TextureHandle(void) = default;
    TextureHandle(const char *filePath);
    ~TextureHandle(void);

    TextureHandle(const TextureHandle &) = delete;
    TextureHandle &operator=(const TextureHandle &) = delete;

    TextureHandle(TextureHandle &&other) noexcept;
    TextureHandle &operator=(TextureHandle &&other) noexcept;
};
