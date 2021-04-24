#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "language.h"

static FILE *header;
static FILE *source;
static i32 count = 0;

static void MakeShader(const char *in, const char *globalVar)
{
    fprintf(header, "extern const char *const %s;\n", globalVar);
    fprintf(source, "const char *const %s = \"\\\n", globalVar);

    FILE *f = fopen(in, "r");
    if (!f)
    {
        fprintf(stderr, "Can't open %s\n", in);
        exit(1);
    }

    char line[4096];
    while (fgets(line, ArrayCount(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        fprintf(source, "%s\\n\\\n", line);
    }

    fputs("\";\n", source);

    count++;
}

int main(void)
{
    header = fopen("../src/shaders.gen.h", "w");
    if (!header)
    {
        perror("Can't open shaders.gen.h");
        return 1;
    }

    source = fopen("../src/shaders.gen.c", "w");
    if (!source)
    {
        perror("Can't open shaders.gen.c");
        return 1;
    }

    fputs("#pragma once\n", header);
    fputs("#include \"shaders.gen.h\"\n", source);

    MakeShader("../src/basic.vert", "BASIC_VERT");
    MakeShader("../src/basic.frag", "BASIC_FRAG");

    fclose(header);
    fclose(source);

    printf("Generated %d shaders\n", count);
}
