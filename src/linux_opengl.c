internal void *OS_LoadOpenGLProcedure(char *name)
{
    void *p = dlsym(globalLibGL, name);

    if (!p)
    {
        return 0;
    }

    return p;
}

internal b32 Linux_WaitForNotify(Display *dpy, XEvent *event, XPointer arg)
{
    return (event->type == MapNotify) && (event->xmap.window == (Window)arg);
}

internal void Linux_CreateWindowWithOpenGL(Window *xWin, GLXWindow *glxWin)
{
    i32 fbAttribs[] = {GLX_DRAWABLE_TYPE,
                       GLX_WINDOW_BIT,
                       GLX_RENDER_TYPE,
                       GLX_RGBA_BIT,
                       GLX_DOUBLEBUFFER,
                       1,
                       GLX_RED_SIZE,
                       1,
                       GLX_GREEN_SIZE,
                       1,
                       GLX_BLUE_SIZE,
                       1,
                       None};
    i32 fbConfigsCount;
    GLXFBConfig *fbConfigs =
        glXChooseFBConfig(globalDpy, DefaultScreen(globalDpy), fbAttribs, &fbConfigsCount);

    XVisualInfo *vInfo = glXGetVisualFromFBConfig(globalDpy, fbConfigs[0]);

    XSetWindowAttributes swa = {0};
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;
    swa.colormap =
        XCreateColormap(globalDpy, RootWindow(globalDpy, vInfo->screen), vInfo->visual, AllocNone);

    i32 swaMask = CWBorderPixel | CWColormap | CWEventMask;

    *xWin = XCreateWindow(globalDpy, RootWindow(globalDpy, vInfo->screen), 0, 0,
                          DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, vInfo->depth, InputOutput,
                          vInfo->visual, swaMask, &swa);

    GLXContext glxContext = glXCreateNewContext(globalDpy, fbConfigs[0], GLX_RGBA_TYPE, NULL, True);

    *glxWin = glXCreateWindow(globalDpy, fbConfigs[0], *xWin, NULL);

    XEvent event;
    XMapWindow(globalDpy, *xWin);
    XIfEvent(globalDpy, &event, Linux_WaitForNotify, (XPointer)*xWin);

    glXMakeContextCurrent(globalDpy, *glxWin, *glxWin, glxContext);
}

internal void OS_GLSwapBuffers(void)
{
    glXSwapBuffers(globalDpy, globalGlxWin);
}
