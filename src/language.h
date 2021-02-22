#define global static
#define internal static
#define persistent static

#define PI 3.1415926535897f

#define Bytes(n) (n)
#define Kilobytes(n) ((n) << 10)
#define Megabytes(n) ((n) << 20)
#define Gigabytes(n) (((u64)(n)) << 30)
#define Terabytes(n) (((u64)(n)) << 40)

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

union v2
{
    v2()
    {
        x = y = 0;
    }

    v2(f32 xx, f32 yy)
    {
        x = xx;
        y = yy;
    }

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

union v3
{
    v3()
    {
        x = y = z = 0;
    }

    v3(f32 xx, f32 yy, f32 zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

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

union v4
{
    v4()
    {
        x = y = z = w = 0;
    }

    v4(f32 xx, f32 yy, f32 zz, f32 ww)
    {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

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

union m4
{
    f32 elements[4][4];
    f32 flatten[16];
};
