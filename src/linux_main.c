#include <X11/Xlib.h>
#include <dlfcn.h>
#include <sys/mman.h>

#include "app.c"

global os_state globalOS;
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
    globalOS.samplesPerSecond = 0;

    globalOS.Reserve = Linux_Reserve;
    globalOS.Release = Linux_Release;
    globalOS.Commit = Linux_Commit;
    globalOS.Decommit = Linux_Decommit;
    globalOS.ReadFile = NULL;
    globalOS.SwapBuffers = Linux_GLSwapBuffers;
    globalOS.LoadOpenGLProcedure = Linux_LoadOpenGLProcedure;

    globalOS.DebugPrint = Linux_DebugPrint;
    globalOS.DebugDisplayError = Linux_DisplayError;

    globalOS.permanentArena = M_ArenaInitialize(Gigabytes(4));
    globalOS.frameArena = M_ArenaInitialize(Gigabytes(4));

    Atom winClosedID = XInternAtom(globalDpy, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(globalDpy, xWin, &winClosedID, 1);

    AppPermanentLoad(&globalOS);

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
