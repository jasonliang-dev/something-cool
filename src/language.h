#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
    #define alignof(T) (int64_t) __alignof(T)
#else
    #include <stdalign.h>
#endif

#ifndef NDEBUG
    #if defined(_WIN32)
        #define DEBUGGER() __debugbreak()
    #elif defined(__linux__)
        #include <signal.h>
        #define DEBUGGER() raise(SIGTRAP)
    #endif
#endif

#ifndef DEBUGGER
    #define DEBUGGER()
#endif

#define Bytes(n) (n)
#define Kilobytes(n) (n << 10)
#define Megabytes(n) (n << 20)
#define Gigabytes(n) (((u64)n) << 30)
#define Terabytes(n) (((u64)n) << 40)

#define PI 3.14159265358979323846f

#define true 1
#define false 0

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Clamp(x, min, max) (((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))

#define Swap(T, a, b)                                                                    \
    do                                                                                   \
    {                                                                                    \
        T temp__ = a;                                                                    \
        a = b;                                                                           \
        b = temp__;                                                                      \
    } while (0);

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;
typedef float f32;
typedef double f64;

static void Fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fputs("\n", stderr);

    abort();
}

/*
typedef struct ReadFileResult ReadFileResult;
struct ReadFileResult
{
    char *data;
    i32 bytes;
};

static ReadFileResult ReadEntireFile(const char *filename, const char *mode)
{
    ReadFileResult result = {NULL, 0};

    FILE *file = fopen(filename, mode);
    if (!file)
    {
        return result;
    }

    fseek(file, 0, SEEK_END);
    result.bytes = ftell(file);

    rewind(file);
    result.data = malloc(result.bytes);
    fread(result.data, 1, result.bytes, file);

    fclose(file);
    return result;
}
*/
