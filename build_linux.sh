#!/bin/sh

set -e

FLAGS="-DDEBUG -std=c++11 \
    -Werror -Wall -Wextra -pedantic-errors \
    -Wno-nested-anon-types -Wno-gnu-anonymous-struct \
"

cd build

clang++ ../src/shadergen.cpp -o shadergen $FLAGS
./shadergen

clang++ -I../third -I../third/glad -I../third/imgui \
    ../src/app.cpp -lSDL2 -lGL -ldl -o app $FLAGS
