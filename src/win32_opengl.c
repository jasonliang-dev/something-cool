#include <gl/gl.h>

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

#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB 0x202B

#define WGL_CHOOSE_PIXEL_FORMAT_ARB(name)                                                          \
    BOOL WINAPI name(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,                \
                     UINT nMaxFormats, int *piFormats, UINT *nNumFormats)
typedef WGL_CHOOSE_PIXEL_FORMAT_ARB(wgl_choose_pixel_format_arb_fn);
global wgl_choose_pixel_format_arb_fn *wglChoosePixelFormatARB;

#define WGL_CREATE_CONTEXT_ATTRIBS_ARB(name)                                                       \
    HGLRC WINAPI name(HDC hdc, HGLRC hShareContext, const int *attribList)
typedef WGL_CREATE_CONTEXT_ATTRIBS_ARB(wgl_create_context_attribs_arb_fn);
global wgl_create_context_attribs_arb_fn *wglCreateContextAttribsARB;

internal void *W32_LoadOpenGLProcedure(char *name)
{
    void *p = (void *)wglGetProcAddress(name);
    if (!p || p == (void *)0x1 || p == (void *)0x2 || p == (void *)0x3 || p == (void *)-1)
    {
        return 0;
    }
    else
    {
        return p;
    }
}

internal HGLRC W32_InitOpenGL(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), // nSize
                                 1,                             // nVersion
                                 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                                     PFD_DOUBLEBUFFER, // dwFlags
                                 PFD_TYPE_RGBA,        // iPixelType
                                 32,                   // cColorBits
                                 0,                    // cRedBits
                                 0,                    // cRedShift
                                 0,                    // cGreenBits
                                 0,                    // cGreenShift
                                 0,                    // cBlueBits
                                 0,                    // cBlueShift
                                 0,                    // cAlphaBits
                                 0,                    // cAlphaShift
                                 0,                    // cAccumBits
                                 0,                    // cAccumRedBits
                                 0,                    // cAccumGreenBits
                                 0,                    // cAccumBlueBits
                                 0,                    // cAccumAlphaBits
                                 24,                   // cDepthBits
                                 8,                    // cStencilBits
                                 0,                    // cAuxBuffers
                                 PFD_MAIN_PLANE,       // iLayerType
                                 0,                    // bReserved
                                 0,                    // dwLayerMask
                                 0,                    // dwVisibleMask
                                 0};                   // dwDamageMask

    i32 pixelFormat = ChoosePixelFormat(hdc, &pfd);

    if (!pixelFormat)
    {
        return NULL;
    }

    SetPixelFormat(hdc, pixelFormat, &pfd);
    HGLRC dummyContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, dummyContext);

    wglChoosePixelFormatARB =
        (wgl_choose_pixel_format_arb_fn *)W32_LoadOpenGLProcedure("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB =
        (wgl_create_context_attribs_arb_fn *)W32_LoadOpenGLProcedure("wglCreateContextAttribsARB");

    i32 pixelFormatAttribs[] = {WGL_DRAW_TO_WINDOW_ARB,
                                GL_TRUE,
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

    if (!pixelFormat || !numFormats)
    {
        return NULL;
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
        return NULL;
    }

    wglMakeCurrent(hdc, 0);
    wglDeleteContext(dummyContext);

    if (!wglMakeCurrent(hdc, glContext))
    {
        return NULL;
    }

    return glContext;
}

internal void W32_CleanUpOpenGL(HDC *hdc, HGLRC glContext)
{
    wglMakeCurrent(*hdc, 0);
    wglDeleteContext(glContext);
}

internal void W32_GLSwapBuffers(void)
{
    wglSwapLayerBuffers(globalHDC, WGL_SWAP_MAIN_PLANE);
}
