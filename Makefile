UNAME_S = $(shell uname -s)

BIN = app
CC = clang

FLAGS = -DDEBUG -DGLFW_INCLUDE_NONE -std=c11 \
	-Werror -Wall -Wextra -pedantic-errors \
	-Wno-missing-braces

SRC := $(wildcard src/*.c)

ifeq ($(UNAME_S), Darwin)
	LD = -framework Cocoa
	SRC := $(wildcard src/*.m) $(filter-out src/win32_%.c, $(SRC))
endif

ifeq ($(UNAME_S), Linux)
	LD =
	SRC :=
endif

.PHONY: all

all: dir app

dir:
	mkdir -p build

app:
	$(CC) $(SRC) $(LD) -o build/$(BIN) $(FLAGS)
