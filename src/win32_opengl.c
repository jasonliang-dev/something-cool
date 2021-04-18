#include "win32_opengl.h"
#include "gl.h"
#include "language.h"

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023

#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_FULL_ACCELERATION_ARB 0x2027

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

global struct
{
    HGLRC context;
    HINSTANCE instance;
} g_GL;

const void *W32_GetProcAddress(const char *name)
{
    void *proc = (void *)wglGetProcAddress(name);

    if (proc == 0 || p == (void *)0x1 || p == (void *)0x2 || p == (void *)0x3 || p == (void *)-1)
    {
        return (void *)GetProcAddress(g_GL.instance, name);
    }

    return proc;
}

b32 W32_InitOpenGL(HDC hdc)
{
    PIXELFORMATDESCRIPTOR desired = {0};
    desired.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    desired.nVersion = 1;
    desired.iPixelType = PFD_TYPE_RGBA;
    desired.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desired.cColorBits = 32;
    desired.cDepthBits = 24;
    desired.cStencilBits = 8;
    desired.iLayerType = PFD_MAIN_PLANE;

    i32 pixelFormat = ChoosePixelFormat(hdc, &desired);

    if (pixelFormat == 0)
    {
        return false;
    }

    SetPixelFormat(hdc, pixelFormat, &desired);

    HGLRC dummyContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, dummyContext);

    wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB || !wglSwapIntervalEXT)
    {
        return false;
    }

    i32 pixelFormatAttribs[] = {WGL_DRAW_TO_WINDOW_ARB,
                                GL_TRUE,
                                WGL_ACCELERATION_ARB,
                                WGL_FULL_ACCELERATION_ARB,
                                WGL_SUPPORT_OPENGL_ARB,
                                GL_TRUE,
                                WGL_DOUBLE_BUFFER_ARB,
                                GL_TRUE,
                                WGL_PIXEL_TYPE_ARB,
                                WGL_TYPE_RGBA_ARB,
                                WGL_COLOR_BITS_ARB,
                                32,
                                WGL_DEPTH_BITS_ARB,
                                24,
                                WGL_STENCIL_BITS_ARB,
                                8,
                                0};

    u32 numFormats = 0;
    wglChoosePixelFormatARB(hdc, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);

    if (pixelFormat == 0 || numFormats == 0)
    {
        return false;
    }

    i32 glAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        3,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        3,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC glContext = wglCreateContextAttribsARB(hdc, dummyContext, glAttribs);

    if (!glContext)
    {
        return false;
    }

    wglMakeCurrent(hdc, 0);
    wglDeleteContext(dummyContext);

    if (!wglMakeCurrent(hdc, glContext))
    {
        return false;
    }

    HINSTANCE opengl32 = LoadLibrary("opengl32.dll");
    if (!opengl32)
    {
        return false;
    }

    g_GL.context = glContext;
    g_GL.instance = opengl32;
    return true;
}
