#/usr/bin/env bash

BUILD_OPTIONS="-DDEBUG=1 -DPLATFORM_LINUX=1"
FLAGS="-std=gnu99 -Werror -Wall -Wextra -Wno-missing-braces -Wno-unused-function"
LIBRARY_LINK="-lm -ldl -lX11 -lGL -lasound"

mkdir -p build
cd build

gcc $FLAGS ../src/linux_main.c $BUILD_OPTIONS -o linux_app $LIBRARY_LINK
