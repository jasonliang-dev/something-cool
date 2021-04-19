#pragma once

#include <X11/Xlib.h>

typedef struct X11_WindowState X11_WindowState;
struct X11_WindowState
{
    Display *display;
    Window window;
    Atom atomWmDeleteWindow;
    b32 quit;
    i32 width;
    i32 height;
};
