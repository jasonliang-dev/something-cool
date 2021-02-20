#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "language.h"

global FILE *output;

void make_shader(const char *in, const char *globalVar)
{
    fprintf(output, "global char *%s =\n", globalVar);

    FILE *f = fopen(in, "r");

    char line[4096];
    while (fgets(line, 4096, f))
    {
        line[strcspn(line, "\n")] = 0;
        fprintf(output, "\"%s\\n\"\n", line);
    }

    fclose(f);

    fputs(";\n", output);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    output = fopen("../src/shaders.gen.h", "w");

    make_shader("../src/quad.vert", "g_quad_vert");
    make_shader("../src/quad.frag", "g_quad_frag");

    fclose(output);
}
