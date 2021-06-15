#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "input.h"
#include "language.h"
#include "opengl_debug.h"
#include "window.h"
#include <glad/gl.h>
#include <stdio.h>

static void ErrorCallback(int code, const char *msg)
{
    fprintf(stderr, "GLFW Error %d: %s\n", code, msg);
}

void InitWindow(void)
{
    glfwSetErrorCallback(ErrorCallback);
    if (!glfwInit())
    {
        Fatal("Cannot initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // Required on Mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    g_Window.handle = glfwCreateWindow(1366, 768, "This is a title", NULL, NULL);
    if (!g_Window.handle)
    {
        Fatal("Cannot create window");
    }

    printf("Created window\n");

    glfwSetKeyCallback(g_Window.handle, KeyboardCallback);
    glfwSetMouseButtonCallback(g_Window.handle, MouseButtonCallback);
    glfwSetCursorPosCallback(g_Window.handle, CursorPositionCallback);
    glfwMakeContextCurrent(g_Window.handle);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        Fatal("Cannot load OpenGL procs");
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

#ifndef NDEBUG
    i32 flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) && glDebugMessageCallback)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugMessageCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                              0, NULL, GL_FALSE);

        printf("Enabled OpenGL debug output\n");
    }
#endif
}
