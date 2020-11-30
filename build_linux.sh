#/usr/bin/env bash

mkdir -p build
cd build

gcc ../src/linux_main.c -lX11 -o linux_app
