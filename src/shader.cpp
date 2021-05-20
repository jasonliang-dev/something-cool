#include "shader.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

Shader::Shader(const char *vert, const char *frag)
{
    auto CompileGLSL = [](GLuint type, const char *source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        b32 success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success)
        {
            return shader;
        }

        i32 length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> message(length);
        glGetShaderInfoLog(shader, length, NULL, message.data());

        const char *typeStr;
        switch (type)
        {
        case GL_VERTEX_SHADER:
            typeStr = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            typeStr = "fragment";
            break;
        default:
            typeStr = "unknown";
        }

        std::stringstream ss;
        ss << "Cannot compile " << typeStr << " shader: " << message.data() << "\n"
           << source;

        throw std::runtime_error(ss.str());
    };

    m_Program = glCreateProgram();

    GLuint vertexShader = CompileGLSL(GL_VERTEX_SHADER, vert);
    GLuint fragmentShader = CompileGLSL(GL_FRAGMENT_SHADER, frag);

    glAttachShader(m_Program, vertexShader);
    glAttachShader(m_Program, fragmentShader);

    glLinkProgram(m_Program);

    b32 linked;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        i32 length;
        glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> message(length);
        glGetProgramInfoLog(m_Program, length, NULL, message.data());

        std::stringstream ss;
        ss << "Cannot link shader. " << message.data() << "\n";

        throw std::runtime_error(ss.str());
    }

    glValidateProgram(m_Program);

    b32 valid;
    glGetProgramiv(m_Program, GL_VALIDATE_STATUS, &valid);

    if (!valid)
    {
        i32 length;
        glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> message(length);
        glGetProgramInfoLog(m_Program, length, NULL, message.data());

        std::stringstream ss;
        ss << "Shader validation failed. " << message.data() << "\n";

        throw std::runtime_error(ss.str());
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader(void)
{
    glDeleteProgram(m_Program);
    m_Program = 0;
}

void Shader::Use(void)
{
    glUseProgram(m_Program);
}

void Shader::SetInt(const char *name, i32 i)
{
    glUniform1i(glGetUniformLocation(m_Program, name), i);
}

void Shader::SetUint(const char *name, u32 u)
{
    glUniform1ui(glGetUniformLocation(m_Program, name), u);
}

void Shader::SetFloat(const char *name, f32 f)
{
    glUniform1f(glGetUniformLocation(m_Program, name), f);
}

void Shader::SetV2(const char *name, v2 v)
{
    glUniform2f(glGetUniformLocation(m_Program, name), v.x, v.y);
}

void Shader::SetV3(const char *name, v3 v)
{
    glUniform3f(glGetUniformLocation(m_Program, name), v.x, v.y, v.z);
}

void Shader::SetV4(const char *name, v4 v)
{
    glUniform4f(glGetUniformLocation(m_Program, name), v.x, v.y, v.z, v.w);
}

void Shader::SetM4(const char *name, m4 m)
{
    glUniformMatrix4fv(glGetUniformLocation(m_Program, name), 1, GL_FALSE, &m[0][0]);
}
