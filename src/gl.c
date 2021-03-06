#ifdef DEBUG
#define GL_CheckForErrors() GL_CheckForErrorsReal(__FILE__, __LINE__)
#else
#define GL_CheckForErrors()
#endif

internal void GL_CheckForErrorsReal(const char *file, u32 line)
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

        LogError("GL_CheckForErrors %s: %s line %d\n", error, file, line);
    }
}

#ifdef DEBUG
void APIENTRY GL_MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar *message, const void *userParam)
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

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_LOW:
        LogError("OpenGL: %s\nfrom: %s, type: %s\n", message, sourceString, typeString);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    default:
        LogInfo("OpenGL: %s\nfrom: %s, type: %s\n", message, sourceString, typeString);
        break;
    }
}
#endif
