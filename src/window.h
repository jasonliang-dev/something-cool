#pragma once

#include "language.h"

b32 WindowCreate(i32 width, i32 height, const char *title);

b32 WindowShouldClose(void);
void WindowPollEvents(void);
void WindowSwapInterval(i32 interval);
void WindowSwapBuffers(void);

i32 WindowWidth(void);
i32 WindowHeight(void);

static v2 WindowV2(void)
{
    return v2((f32)WindowWidth(), (f32)WindowHeight());
}
