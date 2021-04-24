#pragma once

#include "input.h"
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>

void X11_InitInput(Display *display);
i32 X11_TranslateKeyEvent(XKeyEvent *event);
