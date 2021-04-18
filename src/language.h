#pragma once

#include <stdint.h>

#define PI 3.14159265358979323846f

#define true 1
#define false 0

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Max(x, y) (((x) > (y)) ? (x) : (y))

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
