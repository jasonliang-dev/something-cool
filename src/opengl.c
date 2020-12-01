internal void GL_LoadProcedures(void)
{
#define GLProc(name, type) name = (PFN##type##PROC)OS_LoadOpenGLProcedure(#name);
#include "opengl_procedure_list.h"
#undef GLProc
}

internal void GL_CheckForErrorsReal(char *file, u32 line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        char *error = NULL;

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

        if (error)
        {
            char buff[256];
            sprintf(buff, "%s: %s line %d\n", error, file, line);
            OS_DebugPrint(buff);
        }
    }
}

#define GL_CheckForErrors() GL_CheckForErrorsReal(__FILE__, __LINE__)
