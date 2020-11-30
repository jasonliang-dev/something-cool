// Compile with gcc this_file.c -lX11
#include <X11/Xlib.h>

int main(void)
{

    // Create connection to the X Server.
    Display *dpy = XOpenDisplay(NULL);

    // Create window.
    Window win = XCreateSimpleWindow(dpy,
                                     DefaultRootWindow(dpy), // Parent
                                     0, 0,              // X/Y, often overridden by window manager
                                     400, 200,          // Width/Height
                                     0, 0,              // Border
                                     WhitePixel(dpy, 0) // Background
    );

    char msg[] = "Hello, GUI!";

    // Set window title.
    XStoreName(dpy, win, msg);

    // Choose which events we want.
    XSelectInput(dpy, win,
                 SubstructureNotifyMask  // Window size changes (ConfigureNotify)
                     | ExposureMask      // Window needs to be repainted (Expose)
                     | PointerMotionMask // Mouse moves (MotionNotify)
                     | ButtonPressMask | ButtonReleaseMask | KeyPressMask);

    // Make window visible and give it focus.
    XMapRaised(dpy, win);

    // For window manager close button event.
    Atom win_closed_id = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(dpy, win, &win_closed_id, 1);

    for (;;)
    {
        XEvent ev;
        XNextEvent(dpy, &ev);

        // WM close button pushed, exit.
        if (ev.type == ClientMessage && ev.xclient.data.l[0] == win_closed_id)
        {
            return 0;
        }

        // Expose event tells us to repaint (some of) the window.
        if (ev.type == Expose)
        {
            XDrawString(dpy, win, DefaultGC(dpy, 0), 30, 30, msg, sizeof(msg) - 1);
        }
    }
}
