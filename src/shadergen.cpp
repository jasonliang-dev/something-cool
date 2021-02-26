#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "language.h"

global FILE *output;
global i32 count = 0;

internal void MakeShader(const char *in, const char *globalVar)
{
    fprintf(output, "global const char *%s = \"\\\n", globalVar);

    FILE *f = fopen(in, "r");
    if (!f)
    {
        fprintf(stderr, "Can't open %s\n", in);
        fclose(output);
        exit(1);
    }

    char line[4096];
    while (fgets(line, ArrayCount(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        fprintf(output, "%s\\n\\\n", line);
    }

    fclose(f);

    fputs("\";\n", output);

    count++;
}

int main(void)
{
    output = fopen("../src/shaders.gen.h", "w");

    MakeShader("../src/quad.vert", "QUAD_VERT");
    MakeShader("../src/quad.frag", "QUAD_FRAG");
    MakeShader("../src/sprite.frag", "SPRITE_FRAG");
    MakeShader("../src/texture.vert", "TEXTURE_VERT");
    MakeShader("../src/texture.frag", "TEXTURE_FRAG");

    fclose(output);

    printf("Generated %d shaders\n", count);
}
