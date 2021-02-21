#!/bin/sh

IGNORED="-Wno-nested-anon-types -Wno-gnu-anonymous-struct"
FLAGS="-Werror -Wall -Wextra -pedantic-errors $IGNORED"

cd build

clang++ ../src/shadergen.cpp -o shadergen $FLAGS

./shadergen

clang++ -I../third -I../third/gl3w -I../third/imgui \
    ../src/app.cpp -lSDL2 -lGL -ldl -o app $FLAGS
