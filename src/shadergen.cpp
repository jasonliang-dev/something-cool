#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "language.h"

global FILE *output;

internal void MakeShader(const char *in, const char *globalVar)
{
    fprintf(output, "global const char *%s = \"\\\n", globalVar);

    FILE *f = fopen(in, "r");

    char line[4096];
    while (fgets(line, ArrayCount(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        fprintf(output, "%s\\n\\\n", line);
    }

    fclose(f);

    fputs("\";\n", output);
}

int main(void)
{
    output = fopen("../src/shaders.gen.h", "w");

    MakeShader("../src/quad.vert", "QUAD_VERT");
    MakeShader("../src/quad.frag", "QUAD_FRAG");
    MakeShader("../src/sprite.frag", "SPRITE_FRAG");

    fclose(output);
}
