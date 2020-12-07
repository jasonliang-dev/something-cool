#include <X11/Xlib.h>
#include <dlfcn.h>
#include <sys/mman.h>

#include "app.c"

global os_state_t globalOS;
global Display *globalDpy;
global GLXWindow globalGlxWin;
global void *globalLibGL;

#include "linux_opengl.c"
#include "linux_utils.c"

int main(void)
{
    globalDpy = XOpenDisplay(NULL);
    if (!globalDpy)
    {
        printf("Can't open connection to X server\n");
        return 1;
    }

    Window xWin;
    Linux_CreateWindowWithOpenGL(&xWin, &globalGlxWin);
    if (!xWin || !globalGlxWin)
    {
        printf("Can't create window, and/or OpenGL context\n");
        return 1;
    }
    XStoreName(globalDpy, xWin, WINDOW_TITLE);

    globalLibGL = dlopen("libGL.so", RTLD_LAZY);
    if (!globalLibGL)
    {
        printf("Can't open libGL.so\n");
        return 1;
    }

    os = &globalOS;
    globalOS.running = 1;
    globalOS.windowResolution.x = DEFAULT_WINDOW_WIDTH;
    globalOS.windowResolution.y = DEFAULT_WINDOW_HEIGHT;

    globalOS.sampleOut = NULL;
    globalOS.sampleCount = 0;
    globalOS.samplesPerSecond = 0;

    Atom winClosedID = XInternAtom(globalDpy, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(globalDpy, xWin, &winClosedID, 1);

    AppLoad(&globalOS);

    while (globalOS.running)
    {
        XEvent event;
        while (XPending(globalDpy))
        {
            XNextEvent(globalDpy, &event);

            if (event.type == ClientMessage && event.xclient.data.l[0] == winClosedID)
            {
                globalOS.running = 0;
            }
        }

        AppUpdate();
    }

    AppClose();

    return 0;
}
