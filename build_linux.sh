#!/usr/bin/sh

clang++ -Isrc src/app.cpp -lSDL2 -lGL -ldl -o build/app
