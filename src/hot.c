#include <stdio.h>
#include <stdint.h>

#include "language.h"

export void Hello(void)
{
    persistent i32 count = 0;
    printf("%d: Hello World\n", count++);
}
