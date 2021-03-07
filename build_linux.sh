#!/bin/sh

set -e

FLAGS="-DDEBUG -std=c11 \
    -Werror -Wall -Wextra -pedantic-errors \
    -Wno-gnu-anonymous-struct -Wno-missing-braces -Wno-unused-function\
"

cd build

clang ../src/shadergen.c -o shadergen $FLAGS
./shadergen

clang -g -I../third -I../third/glad -I../third/nuklear \
    ../src/app.c -lSDL2 -lGL -ldl -lm -o app $FLAGS
