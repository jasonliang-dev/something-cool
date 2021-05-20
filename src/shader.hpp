#pragma once

#include "language.hpp"
#include <glad/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Shader
{
  public:
    Shader(const char *vert, const char *frag);
    ~Shader(void);

    void Use(void);

    void SetInt(const char *name, i32 i);
    void SetUint(const char *name, u32 u);
    void SetFloat(const char *name, f32 f);
    void SetV2(const char *name, v2 v);
    void SetV3(const char *name, v3 v);
    void SetV4(const char *name, v4 v);
    void SetM4(const char *name, m4 m);

  private:
    GLuint m_Program;
};
