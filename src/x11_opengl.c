#include "x11_opengl.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <limits.h>
#include <string.h>

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display *dpy, GLXFBConfig config,
                                                        GLXContext share_context, Bool direct,
                                                        const int *attrib_list);
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL;

typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display *dpy, GLXDrawable drawable, int interval);
PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;

const void *X11_GetProcAddress(const char *name)
{
    return (void *)glXGetProcAddressARB((const GLubyte *)name);
}

void X11_SwapInterval(Display *display, i32 interval)
{
    if (glXSwapIntervalEXT)
    {
        glXSwapIntervalEXT(display, glXGetCurrentReadDrawable(), interval);
    }
}

void X11_SwapBuffers(Display *display, Window window)
{
    glXSwapBuffers(display, window);
}

static GLXFBConfig X11_GetBestFBC(Display *display, i32 screenID)
{
    GLint glxAttribs[] = {GLX_X_RENDERABLE,
                          True,
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
                          True,
                          None};

    i32 fbcount;
    GLXFBConfig *fbc = glXChooseFBConfig(display, screenID, glxAttribs, &fbcount);
    if (fbc == 0)
    {
        XCloseDisplay(display);
        return false;
    }

    i32 bestFBC = -1;
    i32 worstFBC = -1;
    i32 bestSampleCount = INT_MIN;
    i32 worstSampleCount = INT_MAX;

    for (i32 i = 0; i < fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
        if (vi != 0)
        {
            i32 sampleBuffers;
            i32 samples;
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

            if (bestFBC < 0 || (sampleBuffers && samples > bestSampleCount))
            {
                bestFBC = i;
                bestSampleCount = samples;
            }

            if (worstFBC < 0 || !sampleBuffers || samples < worstSampleCount)
            {
                worstFBC = i;
                worstSampleCount = samples;
            }
        }
        XFree(vi);
    }

    GLXFBConfig bestConfig = fbc[bestFBC];
    XFree(fbc);

    return bestConfig;
}

static GLXContext X11_CreateOpenGLContext(Display *display, GLXFBConfig fbc,
                                          const char *glxExtensions)
{
    i32 contextAttribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,
                            3,
                            GLX_CONTEXT_MINOR_VERSION_ARB,
                            2,
                            GLX_CONTEXT_FLAGS_ARB,
                            GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                            None};

    if (strstr(glxExtensions, "GLX_ARB_create_context") == 0)
    {
        return glXCreateNewContext(display, fbc, GLX_RGBA_TYPE, 0, True);
    }

    glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB(
        (const GLubyte *)"glXCreateContextAttribsARB");
    return glXCreateContextAttribsARB(display, fbc, 0, true, contextAttribs);
}

b32 X11_CreateWindowWithOpenGLContext(X11_WindowState *state, i32 width, i32 height,
                                      const char *title)
{
    Screen *screen;
    i32 screenID;

    Display *display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        return false;
    }

    screen = DefaultScreenOfDisplay(display);
    screenID = DefaultScreen(display);

    GLint majorGLX = 0;
    GLint minorGLX = 0;
    glXQueryVersion(display, &majorGLX, &minorGLX);
    if (majorGLX <= 1 && minorGLX < 2)
    {
        return false;
    }

    GLXFBConfig bestFBC = X11_GetBestFBC(display, screenID);
    XVisualInfo *visual = glXGetVisualFromFBConfig(display, bestFBC);
    if (visual == 0)
    {
        XCloseDisplay(display);
        return false;
    }

    if (screenID != visual->screen)
    {
        XCloseDisplay(display);
        return false;
    }

    XSetWindowAttributes windowAttribs;
    windowAttribs.border_pixel = BlackPixel(display, screenID);
    windowAttribs.background_pixel = WhitePixel(display, screenID);
    windowAttribs.override_redirect = True;
    windowAttribs.colormap =
        XCreateColormap(display, RootWindow(display, screenID), visual->visual, AllocNone);
    windowAttribs.event_mask = ExposureMask;

    Window window = XCreateWindow(
        display, RootWindow(display, screenID), 0, 0, width, height, 0, visual->depth, InputOutput,
        visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);

    Atom atomWMDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &atomWMDeleteWindow, 1);

    const char *glxExtensions = glXQueryExtensionsString(display, screenID);

    GLXContext context = X11_CreateOpenGLContext(display, bestFBC, glxExtensions);
    XSync(display, False);
    glXMakeCurrent(display, window, context);

    if (strstr(glxExtensions, "GLX_EXT_swap_control"))
    {
        glXSwapIntervalEXT =
            (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalEXT");
    }

    XStoreName(display, window, title);
    XClearWindow(display, window);
    XMapRaised(display, window);

    state->display = display;
    state->window = window;
    state->WMDeleteWindow = atomWMDeleteWindow;

    return true;
}
