#!/bin/sh

set -e

FLAGS="-std=c11 -Werror -Wall -Wextra -pedantic-errors"

cd build

clang ../src/run_tests.c -o run_tests $FLAGS
./run_tests
