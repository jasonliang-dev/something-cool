#define global static
#define internal static
#define persistent static

#ifdef _WIN32
#define export __declspec(dllexport)
#else
#define export
#endif

#define PI 3.1415926535897f
#define F32_EPSILON ((f32)FLT_EPSILON)
#define F32_MIN ((f32)FLT_MIN)
#define F32_MAX ((f32)FLT_MAX)

#define Bytes(n) (n)
#define Kilobytes(n) ((n) << 10)
#define Megabytes(n) ((n) << 20)
#define Gigabytes(n) (((u64)(n)) << 30)
#define Terabytes(n) (((u64)(n)) << 40)

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Max(x, y) (((x) > (y)) ? (x) : (y))

#define true 1
#define false 0

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

typedef union v2 v2;
union v2
{
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

    f32 elements[2];
};

typedef union v3 v3;
union v3
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };

    v2 xy;

    struct
    {
        f32 ignore0;
        v2 yz;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };

    f32 elements[3];
};

typedef union v4 v4;
union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                f32 x;
                f32 y;
                f32 z;
            };
        };
        f32 w;
    };

    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                f32 r;
                f32 g;
                f32 b;
            };
        };
        f32 a;
    };

    struct
    {
        f32 ignore0;
        union
        {
            v2 yz;
            v3 yzw;
        };
    };

    struct
    {
        v2 xy;
        union
        {
            v2 zw;
            v2 wh;
            struct
            {
                f32 width;
                f32 height;
            };
        };
    };

    f32 elements[4];
};

typedef union m4 m4;
union m4
{
    f32 elements[4][4];
    f32 flatten[16];
};

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
