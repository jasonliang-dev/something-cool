#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool,
                                                     const int *);
global glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;

global int ctxErrorOccurred = 0;

internal int Linux_CtxErrorHandler(Display *dpy, XErrorEvent *ev)
{
    ctxErrorOccurred = 1;
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

internal void Linux_CreateWindowWithGLContext(Display **display, Window *window)
{
    *display = XOpenDisplay(NULL);

    if (!*display)
    {
        printf("Failed to open X display\n");
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
        printf("Invalid GLX version\n");
        exit(1);
    }
    else
    {
        printf("Using GLX version: %d.%d\n", glxMajor, glxMinor);
    }

    printf("Getting matching framebuffer configs\n");
    int fbcount;
    GLXFBConfig *fbc =
        glXChooseFBConfig(*display, DefaultScreen(*display), visual_attribs, &fbcount);
    printf("Found %d matching FB configs.\n", fbcount);

    printf("Getting XVisualInfos\n");
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    for (int i = 0; i < fbcount; i++)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(*display, fbc[i]);
        if (vi)
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(*display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(*display, fbc[i], GLX_SAMPLES, &samples);

            printf("  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                   " SAMPLES = %d\n",
                   i, vi->visualid, samp_buf, samples);

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
    printf("Chosen visual ID = 0x%x\n", vi->visualid);

    printf("Creating colormap\n");
    XSetWindowAttributes swa;
    Colormap cmap;
    swa.colormap = cmap =
        XCreateColormap(*display, RootWindow(*display, vi->screen), vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    printf("Creating window\n");
    *window =
        XCreateWindow(*display, RootWindow(*display, vi->screen), 0, 0, 100, 100, 0, vi->depth,
                      InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
    if (!*window)
    {
        printf("Failed to create window.\n");
        exit(1);
    }

    XFree(vi);

    XStoreName(*display, *window, "GL 3.0 Window");

    printf("Mapping window\n");
    XMapWindow(*display, *window);

    const char *glxExts = glXQueryExtensionsString(*display, DefaultScreen(*display));

    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
        (const GLubyte *)"glXCreateContextAttribsARB");

    GLXContext ctx = 0;

    ctxErrorOccurred = 0;
    int (*oldHandler)(Display *, XErrorEvent *) = XSetErrorHandler(&Linux_CtxErrorHandler);

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        // GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None};

    printf("Creating context\n");
    ctx = glXCreateContextAttribsARB(*display, bestFbc, 0, 1, context_attribs);

    XSync(*display, 0);
    if (!ctxErrorOccurred && ctx)
    {
        printf("Created GL 3.3 context\n");
    }
    else
    {
        printf("Failed to create GL 3.3 context\n");
        exit(1);
    }

    XSync(*display, 0);

    XSetErrorHandler(oldHandler);

    if (ctxErrorOccurred || !ctx)
    {
        printf("Failed to create an OpenGL context\n");
        exit(1);
    }

    glXMakeCurrent(*display, *window, ctx);
}

internal void OS_GLSwapBuffers(void)
{
    glXSwapBuffers(globalDisplay, globalWindow);
}
