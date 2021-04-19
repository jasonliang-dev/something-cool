UNAME_S = $(shell uname -s)

BIN = app
CC = clang

FLAGS = -DDEBUG -DGLFW_INCLUDE_NONE -std=c11 \
	-Werror -Wall -Wextra -pedantic-errors \
	-Wno-missing-braces

SRC := $(filter-out src/win32_%.c, $(wildcard src/*.c))

.PHONY: all dir app

all: dir app

dir:
	mkdir -p build

ifeq ($(UNAME_S), Darwin)

LD = -framework Cocoa -framework OpenGL
SRC := $(wildcard src/*.m) $(SRC)

app:
	mkdir -p build/$(BIN).app/Contents/MacOS/
	$(CC) $(SRC) $(LD) -o build/$(BIN).app/Contents/MacOS/$(BIN) $(FLAGS)

endif
