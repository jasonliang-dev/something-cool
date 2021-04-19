#include "input.h"
#include <stdio.h>

const char *KeyToString(i32 key)
{
    switch (key)
    {
#define X(name, string)                                                                            \
    case name:                                                                                     \
        return string;
        INPUT_KEY_LIST
#undef X
    }

    return KeyToString(Key_Unknown);
}

void OnKeyPress(i32 key)
{
    printf("%s pressed\n", KeyToString(key));
}

void OnKeyRelease(i32 key)
{
    printf("%s released\n", KeyToString(key));
}

void OnKeyRepeat(i32 key)
{
    printf("%s repeat\n", KeyToString(key));
}

void OnMouseClick(i32 button)
{
    printf("%d clicked\n", button);
}

void OnMouseRelease(i32 button)
{
    printf("%d released\n", button);
}

void OnMouseMove(i32 x, i32 y)
{
    printf("mouse: %d, %d\n", x, y);
}
