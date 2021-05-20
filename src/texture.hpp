#pragma once

#include "language.hpp"
#include <glad/gl.h>

class Texture
{
  public:
    Texture(const char *filePath);
    ~Texture(void);

    void Bind(i32 unit);

    i32 GetWidth(void) const;
    i32 GetHeight(void) const;

  private:
    GLuint m_ID;
    i32 m_Width;
    i32 m_Height;
};
