#pragma once

#include "geometry.h"
#include "gl.h"
#include "language.h"

typedef GLuint Shader;

Shader ShaderCreate(const char *vert, const char *frag); // binds shader (glUseProgram) on create
void ShaderDestory(Shader program);
void ShaderUse(Shader program);

// uniforms
void ShaderSetInt(Shader program, const char *name, i32 i);
void ShaderSetUint(Shader program, const char *name, u32 u);
void ShaderSetFloat(Shader program, const char *name, f32 f);
void ShaderSetV2(Shader program, const char *name, v2 v);
void ShaderSetV3(Shader program, const char *name, v3 v);
void ShaderSetV4(Shader program, const char *name, v4 v);
void ShaderSetM4(Shader program, const char *name, m4 m);
