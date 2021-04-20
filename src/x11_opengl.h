#pragma once

#include "language.h"
#include "x11_window_state.h"

const void *X11_GetProcAddress(const char *name);
void X11_SwapInterval(Display *display, i32 interval);
void X11_SwapBuffers(Display *display, Window window);
b32 X11_CreateWindowWithOpenGLContext(X11_WindowState *state, i32 width, i32 height,
                                      const char *title);
