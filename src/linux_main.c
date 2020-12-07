#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>

#include "app.c"

global os_state_t globalOS;
global Display *globalDisplay;
global Window globalWindow;
global void *globalLibGL;

#include "linux_utils.c"
#include "linux_opengl.c"

int main(int argc, char *argv[])
{
    GLXContext ctx;
    Colormap cmap;
    Linux_CreateWindowWithGLContext(&globalDisplay, &globalWindow, &ctx, &cmap);

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

    Atom winClosedID = XInternAtom(globalDisplay, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(globalDisplay, globalWindow, &winClosedID, 1);

    AppLoad(&globalOS);

    while (globalOS.running)
    {
        XEvent event;
        while (XPending(globalDisplay))
        {
            XNextEvent(globalDisplay, &event);

            if (event.type == ClientMessage && event.xclient.data.l[0] == winClosedID)
            {
                globalOS.running = 0;
            }
        }

        AppUpdate();
    }

    AppClose();

    glXMakeCurrent(globalDisplay, 0, 0);
    glXDestroyContext(globalDisplay, ctx);

    XDestroyWindow(globalDisplay, globalWindow);
    XFreeColormap(globalDisplay, cmap);
    XCloseDisplay(globalDisplay);

    return 0;
}
