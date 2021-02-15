#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#include "app.c"

global os_state_t globalOS;
global Display *globalDisplay;
global Window globalWindow;
global void *globalLibGL;

#include "linux_os.c"
#include "linux_opengl.c"
#include "linux_alsa.c"
#include "linux_file_io.c"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    GLXContext ctx;
    Colormap cmap;

    if (!Linux_CreateWindowWithGLContext(&globalDisplay, &globalWindow, &ctx, &cmap))
    {
        printf("Can't create window\n");
        return 1;
    }

    globalLibGL = dlopen("libGL.so", RTLD_LAZY);
    if (!globalLibGL)
    {
        printf("Can't open libGL.so\n");
        return 1;
    }

    linux_sound_output_t soundOutput = {0};
    soundOutput.channels = 2;
    soundOutput.samplesPerSecond = 48000;
    Linux_InitAlsa(&soundOutput);

    os = &globalOS;
    globalOS.running = 1;
    globalOS.windowResolution.x = DEFAULT_WINDOW_WIDTH;
    globalOS.windowResolution.y = DEFAULT_WINDOW_HEIGHT;

    globalOS.sampleCount = 1000;
    globalOS.sampleOut = malloc(globalOS.sampleCount * sizeof(i16) * 2);

    globalOS.samplesPerSecond = soundOutput.samplesPerSecond;

    Atom winClosedID = XInternAtom(globalDisplay, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(globalDisplay, globalWindow, &winClosedID, 1);

    AppLoad(&globalOS);

    while (globalOS.running)
    {
        XEvent event;
        while (XPending(globalDisplay))
        {
            XNextEvent(globalDisplay, &event);

            if (event.type == ClientMessage && (u64)event.xclient.data.l[0] == winClosedID)
            {
                globalOS.running = 0;
            }
        }

        AppUpdate();
        Linux_FillSoundBuffer(globalOS.sampleCount, globalOS.sampleOut, &soundOutput);
    }

    AppClose();

    glXMakeCurrent(globalDisplay, 0, 0);
    glXDestroyContext(globalDisplay, ctx);

    XDestroyWindow(globalDisplay, globalWindow);
    XFreeColormap(globalDisplay, cmap);
    XCloseDisplay(globalDisplay);

    return 0;
}
