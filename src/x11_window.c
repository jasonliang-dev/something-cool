#include "gl.h"
#include "input.h"
#include "window.h"
#include "x11_input.h"
#include "x11_opengl.h"
#include "x11_window_state.h"
#include <stdio.h>

static X11_WindowState g_Window;

b32 WindowCreate(i32 width, i32 height, const char *title)
{
    if (!X11_CreateWindowWithOpenGLContext(&g_Window, width, height, title))
    {
        return false;
    }

    LoadOpenGLProcs(X11_GetProcAddress);
    X11_InitInput(g_Window.display);

    XFlush(g_Window.display);
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
            OnKeyPress(X11_TranslateKeyEvent((XKeyEvent *)&event));
            break;
        case KeyRelease:
            if (XEventsQueued(g_Window.display, QueuedAfterReading))
            {
                XEvent nextEvent;
                XPeekEvent(g_Window.display, &nextEvent);

                if (nextEvent.type == KeyPress && event.xkey.time == nextEvent.xkey.time &&
                    event.xkey.keycode == nextEvent.xkey.keycode)
                {
                    XNextEvent(g_Window.display, &nextEvent);
                    OnKeyRepeat(X11_TranslateKeyEvent((XKeyEvent *)&event));
                    break;
                }
            }

            OnKeyRelease(X11_TranslateKeyEvent((XKeyEvent *)&event));
            break;
        default:
            break;
        }
    }
}

void WindowSwapInterval(i32 interval)
{
    X11_SwapInterval(g_Window.display, interval);
}

void WindowSwapBuffers(void)
{
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
