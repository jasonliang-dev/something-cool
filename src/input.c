#include "input.h"
#include <stdio.h>

void OnKeyPress(i32 key)
{
    printf("%d pressed\n", key);
}

void OnKeyRelease(i32 key)
{
    printf("%d released\n", key);
}

void OnKeyRepeat(i32 key)
{
    printf("%d repeat\n", key);
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
