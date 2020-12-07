#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool,
                                                     const int *);
global glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;

global int globalCtxErrorOccurred = 0;

internal int Linux_CtxErrorHandler(Display *dpy, XErrorEvent *ev)
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

internal void Linux_CreateWindowWithGLContext(Display **display, Window *window, GLXContext *ctx,
                                              Colormap *cmap)
{
    *display = XOpenDisplay(NULL);

    if (!*display)
    {
        exit(1);
    }

    static int visual_attribs[] = {GLX_X_RENDERABLE,
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

    int glxMajor, glxMinor;

    if (!glXQueryVersion(*display, &glxMajor, &glxMinor) || ((glxMajor == 1) && (glxMinor < 3)) ||
        (glxMajor < 1))
    {
        exit(1);
    }

    int fbcount;
    GLXFBConfig *fbc =
        glXChooseFBConfig(*display, DefaultScreen(*display), visual_attribs, &fbcount);

    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    for (int i = 0; i < fbcount; i++)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(*display, fbc[i]);
        if (vi)
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(*display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(*display, fbc[i], GLX_SAMPLES, &samples);

            if (best_fbc < 0 || samp_buf && samples > best_num_samp)
            {
                best_fbc = i, best_num_samp = samples;
            }
            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
            {
                worst_fbc = i, worst_num_samp = samples;
            }
        }
        XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[best_fbc];

    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(*display, bestFbc);

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
        exit(1);
    }

    XFree(vi);

    XStoreName(*display, *window, "GL 3.0 Window");

    XMapWindow(*display, *window);

    const char *glxExts = glXQueryExtensionsString(*display, DefaultScreen(*display));

    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
        (const GLubyte *)"glXCreateContextAttribsARB");

    *ctx = 0;

    globalCtxErrorOccurred = 0;
    int (*oldHandler)(Display *, XErrorEvent *) = XSetErrorHandler(&Linux_CtxErrorHandler);

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        // GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None};

    *ctx = glXCreateContextAttribsARB(*display, bestFbc, 0, 1, context_attribs);

    XSync(*display, 0);
    if (globalCtxErrorOccurred || !*ctx)
    {
        exit(1);
    }

    XSync(*display, 0);

    XSetErrorHandler(oldHandler);

    if (globalCtxErrorOccurred || !*ctx)
    {
        exit(1);
    }

    glXMakeCurrent(*display, *window, *ctx);
}

internal void OS_GLSwapBuffers(void)
{
    glXSwapBuffers(globalDisplay, globalWindow);
}
