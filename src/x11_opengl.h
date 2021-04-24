#pragma once

#include "language.h"
#include <X11/Xlib.h>

const void *X11_GetProcAddress(const char *name);
void X11_SwapInterval(Display *display, i32 interval);
void X11_SwapBuffers(Display *display, Window window);
b32 X11_CreateWindowWithOpenGLContext(Display **displayP, Window *windowP,
                                      Atom *atomWMDeleteWindowP, i32 eventMask, i32 width,
                                      i32 height, const char *title);
