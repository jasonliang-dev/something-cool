#include "shader.h"
#include <stdio.h>

static GLuint CompileGLSL(GLuint type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    i32 result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        i32 length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char message[512];
        length = Max((u32)length, sizeof(message));
        glGetShaderInfoLog(shader, length, NULL, message);

        fprintf(stderr, "Cannot compile shader %s\n%s\n", message, source);
    }

    return shader;
}

Shader ShaderCreate(const char *vert, const char *frag)
{
    Shader program = glCreateProgram();
    GLuint vertexShader = CompileGLSL(GL_VERTEX_SHADER, vert);
    GLuint fragmentShader = CompileGLSL(GL_FRAGMENT_SHADER, frag);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    ShaderUse(program);
    return program;
}

void ShaderDestory(Shader program)
{
    glDeleteProgram(program);
}

void ShaderUse(Shader program)
{
    glUseProgram(program);
}

void ShaderSetInt(Shader program, const char *name, i32 i)
{
    glUniform1i(glGetUniformLocation(program, name), i);
}

void ShaderSetUint(Shader program, const char *name, u32 u)
{
    glUniform1ui(glGetUniformLocation(program, name), u);
}

void ShaderSetFloat(Shader program, const char *name, f32 f)
{
    glUniform1f(glGetUniformLocation(program, name), f);
}

void ShaderSetV2(Shader program, const char *name, v2 v)
{
    glUniform2f(glGetUniformLocation(program, name), v.x, v.y);
}

void ShaderSetV3(Shader program, const char *name, v3 v)
{
    glUniform3f(glGetUniformLocation(program, name), v.x, v.y, v.z);
}

void ShaderSetV4(Shader program, const char *name, v4 v)
{
    glUniform4f(glGetUniformLocation(program, name), v.x, v.y, v.z, v.w);
}

void ShaderSetM4(Shader program, const char *name, m4 m)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, m.flatten);
}
