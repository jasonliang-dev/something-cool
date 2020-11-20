#include <stdint.h>

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

typedef union {
    struct
    {
        f32 x;
        f32 y;
    };

    struct
    {
        f32 width;
        f32 height;
    };

    float elements[2];
} v2;

typedef union {
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };

    f32 elements[3];
} v3;

typedef union {
    struct
    {
        f32 x;
        f32 y;
        union {
            struct
            {
                f32 z;
                f32 w;
            };
            struct
            {
                f32 width;
                f32 height;
            };
        };
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };

    f32 elements[4];
} v4;

typedef struct
{
    f32 elements[4][4];
} m4;

#define v2(...)                                                                                    \
    (v2)                                                                                           \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define v3(...)                                                                                    \
    (v3)                                                                                           \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define v4(...)                                                                                    \
    (v4)                                                                                           \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
