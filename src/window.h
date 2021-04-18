#pragma once

#include "language.h"

b32 WindowCreate(i32 width, i32 height, const char *title);

b32 WindowShouldClose(void);
void WindowPollEvents(void);
void WindowSwapInterval(i32 interval);
void WindowSwapBuffers(void);
void WindowBeginDraw(void);

i32 Window_Width(void);
i32 Window_Height(void);
