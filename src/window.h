#pragma once

#include "language.h"

typedef struct WindowState WindowState;
struct WindowState
{
    b32 quit;
    i32 width;
    i32 height;
};

extern WindowState window;

b32 WindowCreate(i32 width, i32 height, const char *title);
void *WindowGetHandle(void);

b32 WindowIsFocused(void);
void WindowPollEvents(void);
void WindowSwapInterval(i32 interval);
void WindowSwapBuffers(void);

static v2 WindowV2(void)
{
    return v2((f32)window.width, (f32)window.height);
}
