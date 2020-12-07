#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>

#include "app.c"

global Display *globalDisplay;
global Window globalWindow;
global void *globalLibGL;

#include "linux_utils.c"
#include "linux_opengl.c"

int main(int argc, char *argv[])
{
    Linux_CreateWindowWithGLContext(&globalDisplay, &globalWindow);

    globalLibGL = dlopen("libGL.so", RTLD_LAZY);
    if (!globalLibGL)
    {
        printf("Can't open libGL.so\n");
        return 1;
    }

    glClearColor(0, 0.5, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    OS_GLSwapBuffers();

    sleep(1);

    glClearColor(1, 0.5, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glXSwapBuffers(globalDisplay, globalWindow);

    sleep(1);

    /*
    glXMakeCurrent(globalDisplay, 0, 0);
    glXDestroyContext(globalDisplay, ctx);

    XDestroyWindow(globalDisplay, globalWindow);
    XFreeColormap(globalDisplay, cmap);
    XCloseDisplay(globalDisplay);
    */

    return 0;
}
