#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet memset
#define CStringLength (u32) strlen
#define FMod fmodf
#define AbsoluteValue fabsf
#define SquareRoot sqrtf
#define Sin sinf
#define Cos cosf
#define Tan tanf

#define global static
#define internal static
#define local_persist static
#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define Bytes(n) (n)
#define Kilobytes(n) (n << 10)
#define Megabytes(n) (n << 20)
#define Gigabytes(n) (((u64)n) << 30)
#define Terabytes(n) (((u64)n) << 40)
#define PI (3.1415926535897f)

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

#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Max(x, y) (((x) > (y)) ? (x) : (y))

#if DEBUG
#define Assert(b)                                                                                  \
    if (!(b))                                                                                      \
    {                                                                                              \
        *(int *)0 = 0;                                                                             \
    }
#else
#define Assert(b)
#endif
