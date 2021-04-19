UNAME_S = $(shell uname -s)

BIN = app
CC = clang

FLAGS = -DDEBUG -DGLFW_INCLUDE_NONE -std=c11 \
	-Werror -Wall -Wextra -pedantic-errors \
	-Wno-missing-braces

SRC := $(filter-out src/win32_%.c, $(wildcard src/*.c))

.PHONY: all

# build for macos
ifeq ($(UNAME_S), Darwin)

LIB = -framework Cocoa -framework OpenGL
SRC := $(wildcard src/*.m) $(filter-out src/x11_%.c, $(SRC))

all:
	mkdir -p build/$(BIN).app/Contents/MacOS/
	$(CC) $(SRC) $(LIB) -o build/$(BIN).app/Contents/MacOS/$(BIN) $(FLAGS)

endif

# build for linux
ifeq ($(UNAME_S), Linux)

LIB = -lX11 -lGL -lm

all:
	mkdir -p build
	$(CC) $(SRC) $(LIB) -o build/$(BIN) $(FLAGS)

endif
