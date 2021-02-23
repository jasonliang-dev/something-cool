#define GL_CheckForErrors() GL_CheckForErrorsReal(__FILE__, __LINE__)

internal void GL_CheckForErrorsReal(const char *file, u32 line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char *error = nullptr;

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
        }

        assert(error);
        DisplayError("%s: %s line %d\n", error, file, line);
    }
}

#ifdef DEBUG
void APIENTRY GL_MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar *message, const void *userParam)
{
    (void)source;
    (void)type;
    (void)id;
    (void)length;
    (void)userParam;

    const char *severityString = nullptr;

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
    }

    assert(severityString);
    DisplayError("OpenGL (%s): %s\n", severityString, message);
}
#endif
