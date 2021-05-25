#pragma once

#include "language.hpp"
#include "geometry.hpp"
#include <glad/gl.h>

struct Texture
{
    Texture(void) = default;
    Texture(const char *filePath);
    ~Texture(void);

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    void Bind(i32 unit);

    v2 GetDim(void) const;

    GLuint m_ID = 0;
    i32 m_Width = 0;
    i32 m_Height = 0;
};
