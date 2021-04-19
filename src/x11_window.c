#include "gl.h"
#include "window.h"
#include "x11_opengl.h"
#include "x11_window_state.h"

static X11_WindowState g_Window;

b32 WindowCreate(i32 width, i32 height, const char *title)
{
    if (!X11_CreateWindowWithOpenGLContext(&g_Window, width, height, title))
    {
        return false;
    }

    LoadOpenGLProcs(X11_GetProcAddress);

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
            if (event.xclient.data.l[0] == (long)g_Window.atomWmDeleteWindow)
            {
                g_Window.quit = true;
            }
            break;
        case DestroyNotify:
            g_Window.quit = true;
            break;
        default:
            break;
        }
    }
}

void WindowSwapInterval(i32 interval)
{
    (void)interval;
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
