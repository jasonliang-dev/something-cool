#/usr/bin/env bash

BUILD_OPTIONS="-DDEBUG=1 -DBUILD_LINUX=1"
FLAGS="-std=gnu99"
LIBRARY_LINK="-lm -ldl -lX11 -lGL"

mkdir -p build
cd build

gcc $FLAGS ../src/linux_main.c $BUILD_OPTIONS -o linux_app $LIBRARY_LINK