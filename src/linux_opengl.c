#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool,
                                                     const int *);
global glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;

global b32 globalCtxErrorOccurred = 0;

internal i32 Linux_CtxErrorHandler(Display *dpy, XErrorEvent *ev)
{
    globalCtxErrorOccurred = 1;
    return 0;
}

internal void *OS_LoadOpenGLProcedure(char *name)
{
    void *p = dlsym(globalLibGL, name);

    if (!p)
    {
        return 0;
    }

    return p;
}

internal b32 Linux_CreateWindowWithGLContext(Display **display, Window *window, GLXContext *ctx,
                                             Colormap *cmap)
{
    *display = XOpenDisplay(NULL);

    if (!*display)
    {
        return 0;
    }

    static i32 visual_attribs[] = {GLX_X_RENDERABLE,
                                   1,
                                   GLX_DRAWABLE_TYPE,
                                   GLX_WINDOW_BIT,
                                   GLX_RENDER_TYPE,
                                   GLX_RGBA_BIT,
                                   GLX_X_VISUAL_TYPE,
                                   GLX_TRUE_COLOR,
                                   GLX_RED_SIZE,
                                   8,
                                   GLX_GREEN_SIZE,
                                   8,
                                   GLX_BLUE_SIZE,
                                   8,
                                   GLX_ALPHA_SIZE,
                                   8,
                                   GLX_DEPTH_SIZE,
                                   24,
                                   GLX_STENCIL_SIZE,
                                   8,
                                   GLX_DOUBLEBUFFER,
                                   1,
                                   None};

    i32 glxMajor, glxMinor;

    if (!glXQueryVersion(*display, &glxMajor, &glxMinor) || ((glxMajor == 1) && (glxMinor < 3)) ||
        (glxMajor < 1))
    {
        return 0;
    }

    i32 fbcount;
    GLXFBConfig *fbc =
        glXChooseFBConfig(*display, DefaultScreen(*display), visual_attribs, &fbcount);

    // from http://docs.gl/gl3/glBlitFramebuffer:

    // GL_INVALID_OPERATION is generated if GL_SAMPLE_BUFFERS for both read and
    // draw buffers greater than zero and the dimensions of the source and
    // destination rectangles is not identical.

    // app->screenFBO has 0 sample buffers, while on linux, the default
    // framebuffer could have 0 or 1 sample buffers.

    // so I could find the best fbc by finding the fbc with the highest samples
    // and sample buffers, but what I really need is a fbc with a sample buffer
    // count of 0.

    i32 bestFbc = -1;
    i32 worstFbc = -1;
    i32 bestNumSamp = -1;
    i32 worstNumSamp = 999;

    for (i32 i = 0; i < fbcount; i++)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(*display, fbc[i]);
        if (vi)
        {
            i32 sampBuff, samples;
            glXGetFBConfigAttrib(*display, fbc[i], GLX_SAMPLE_BUFFERS, &sampBuff);
            glXGetFBConfigAttrib(*display, fbc[i], GLX_SAMPLES, &samples);

            if (bestFbc < 0 || (!sampBuff && samples > bestNumSamp))
            {
                bestFbc = i;
                bestNumSamp = samples;
            }

            if (worstFbc < 0 || sampBuff || samples < worstNumSamp)
            {
                worstFbc = i;
                worstNumSamp = samples;
            }
        }
        XFree(vi);
    }

    GLXFBConfig useFbc = fbc[bestFbc];

    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(*display, useFbc);

    XSetWindowAttributes swa;
    swa.colormap = *cmap =
        XCreateColormap(*display, RootWindow(*display, vi->screen), vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    *window = XCreateWindow(*display, RootWindow(*display, vi->screen), 0, 0, DEFAULT_WINDOW_WIDTH,
                            DEFAULT_WINDOW_HEIGHT, 0, vi->depth, InputOutput, vi->visual,
                            CWBorderPixel | CWColormap | CWEventMask, &swa);
    if (!*window)
    {
        return 0;
    }

    XFree(vi);

    XStoreName(*display, *window, WINDOW_TITLE);

    XMapWindow(*display, *window);

    const char *glxExts = glXQueryExtensionsString(*display, DefaultScreen(*display));

    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
        (const GLubyte *)"glXCreateContextAttribsARB");

    *ctx = 0;

    globalCtxErrorOccurred = 0;
    i32 (*oldHandler)(Display *, XErrorEvent *) = XSetErrorHandler(&Linux_CtxErrorHandler);

    i32 context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        // GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None};

    *ctx = glXCreateContextAttribsARB(*display, useFbc, 0, 1, context_attribs);

    XSync(*display, 0);
    if (globalCtxErrorOccurred || !*ctx)
    {
        return 0;
    }

    XSync(*display, 0);

    XSetErrorHandler(oldHandler);

    if (globalCtxErrorOccurred || !*ctx)
    {
        return 0;
    }

    glXMakeCurrent(*display, *window, *ctx);

    return 1;
}

internal void OS_GLSwapBuffers(void)
{
    glXSwapBuffers(globalDisplay, globalWindow);
}
