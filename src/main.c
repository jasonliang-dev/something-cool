#include "gl.h"
#include "window.h"
#include "texture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    if (!WindowCreate(1366, 768, "This is a title"))
    {
        fprintf(stderr, "Failed to create a window\n");
        abort();
    }

    WindowSwapInterval(1);

    Texture tex = TextureCreate("data/test.bmp");
    (void)tex;

    while (!WindowShouldClose())
    {
        static f32 s_Time = 0.0f;
        s_Time += 0.05f;

        glClearColor(sinf(s_Time) * 0.5f + 0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        WindowSwapBuffers();
        WindowPollEvents();
    }
}
