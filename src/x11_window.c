#include "gl.h"
#include "input.h"
#include "window.h"
#include "x11_input.h"
#include "x11_opengl.h"
#include <X11/Xlib.h>

static struct
{
    Display *display;
    Window window;
    Atom WMDeleteWindow;
    b32 quit;
    i32 width;
    i32 height;
} g_Window;

b32 WindowCreate(i32 width, i32 height, const char *title)
{
    i32 eventMask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                    ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    if (!X11_CreateWindowWithOpenGLContext(&g_Window.display, &g_Window.window,
                                           &g_Window.WMDeleteWindow, eventMask, width, height,
                                           title))
    {
        return false;
    }

    LoadOpenGLProcs(X11_GetProcAddress);
    X11_InitInput(g_Window.display);

    XFlush(g_Window.display);

    g_Window.quit = false;
    g_Window.width = width;
    g_Window.height = height;

    return true;
}

b32 WindowShouldClose(void)
{
    return g_Window.quit;
}

void WindowPollEvents(void)
{
    XEvent event;

    while (XPending(g_Window.display) > 0)
    {
        XNextEvent(g_Window.display, &event);

        switch (event.type)
        {
        case Expose: {
            XWindowAttributes attribs;
            XGetWindowAttributes(g_Window.display, g_Window.window, &attribs);
            glViewport(0, 0, attribs.width, attribs.height);
            g_Window.width = attribs.width;
            g_Window.height = attribs.height;
            break;
        }
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == g_Window.WMDeleteWindow)
            {
                g_Window.quit = true;
            }
            break;
        case DestroyNotify:
            g_Window.quit = true;
            break;
        case KeyPress:
            OnKeyPress(X11_TranslateKeyEvent(&event.xkey));
            break;
        case KeyRelease:
            if (XEventsQueued(g_Window.display, QueuedAfterReading))
            {
                XEvent nextEvent;
                XPeekEvent(g_Window.display, &nextEvent);

                if (nextEvent.type == KeyPress && event.xkey.time == nextEvent.xkey.time &&
                    event.xkey.keycode == nextEvent.xkey.keycode)
                {
                    // key isn't actually released.
                    XNextEvent(g_Window.display, &nextEvent);
                    OnKeyRepeat(X11_TranslateKeyEvent(&event.xkey));
                    break;
                }
            }

            OnKeyRelease(X11_TranslateKeyEvent(&event.xkey));
            break;
        case MotionNotify:
            OnMouseMove(event.xmotion.x, event.xmotion.y);
            break;
        case ButtonPress:
            OnMouseClick(X11_TranslateButtonEvent(&event.xbutton));
            break;
        case ButtonRelease:
            OnMouseRelease(X11_TranslateButtonEvent(&event.xbutton));
            break;
        default:
            break;
        }
    }
}

// wrapping calls to glXSwapIntervalEXT and glXSwapBuffers because
// GL/glx.h doesn't like the gl.h header.

void WindowSwapInterval(i32 interval)
{
    // glXSwapIntervalEXT
    X11_SwapInterval(g_Window.display, interval);
}

void WindowSwapBuffers(void)
{
    // glXSwapBuffers
    X11_SwapBuffers(g_Window.display, g_Window.window);
}

i32 Window_Width(void)
{
    return g_Window.width;
}

i32 Window_Height(void)
{
    return g_Window.height;
}
