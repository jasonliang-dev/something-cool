#pragma once

#include <glad/gl.h>
#include <iostream>

#define GL_CheckForErrors() GL_CheckForErrorsReal(__FILE__, __LINE__)

static void GL_CheckForErrorsReal(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char *error = NULL;

        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "UNKNOWN_ERROR";
            break;
        }

        std::cerr << "GL_CheckForErrors " << error << ": " << file << " line " << line
                  << "\n";
    }
}

static void GL_DebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar *message, const void *userParam)
{
    (void)id;
    (void)length;
    (void)userParam;

    const char *sourceString;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        sourceString = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceString = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceString = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceString = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        sourceString = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
    default:
        sourceString = "UNKNOWN";
        break;
    }

    const char *typeString;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        typeString = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeString = "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeString = "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeString = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeString = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
    default:
        typeString = "OTHER";
        break;
    }

    const char *severityString;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        severityString = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severityString = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severityString = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityString = "NOTIFICATION";
        break;
    default:
        severityString = "UNKNOWN";
        break;
    }

    std::cerr << "OpenGL: " << sourceString << ": " << typeString << ": " << message
              << "\n"
              << "Severity: " << severityString << "\n";
}
