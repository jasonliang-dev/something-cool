#define global static
#define internal static
#define persistent static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

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

    float elements[2];
};

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
        f32 ignore1;
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
        v2 yz;
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
