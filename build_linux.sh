#!/bin/sh

set -e

FLAGS="-DDEBUG -std=c++11 \
    -Werror -Wall -Wextra -pedantic-errors \
    -Wno-gnu-anonymous-struct \
"

cd build

clang ../src/shadergen.c -o shadergen $FLAGS
./shadergen

clang -I../third -I../third/glad -I../third/nuklear \
    ../src/app.c -lSDL2 -lGL -ldl -o app $FLAGS
