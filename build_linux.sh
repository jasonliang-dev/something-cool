#!/usr/bin/sh

clang++ -Isrc src/main.cpp -lSDL2 -lGL -ldl -o build/app
