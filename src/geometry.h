#pragma once

#include "language.h"

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

typedef union iv2 iv2;
union iv2
{
    struct
    {
        i32 x;
        i32 y;
    };

    struct
    {
        i32 width;
        i32 height;
    };

    i32 elements[2];
};

typedef union iv3 iv3;
union iv3
{
    struct
    {
        i32 x;
        i32 y;
        i32 z;
    };

    iv2 xy;

    struct
    {
        i32 ignore0;
        iv2 yz;
    };

    struct
    {
        i32 r;
        i32 g;
        i32 b;
    };

    i32 elements[3];
};

typedef union iv4 iv4;
union iv4
{
    struct
    {
        union
        {
            iv3 xyz;
            struct
            {
                i32 x;
                i32 y;
                i32 z;
            };
        };
        i32 w;
    };

    struct
    {
        union
        {
            iv3 rgb;
            struct
            {
                i32 r;
                i32 g;
                i32 b;
            };
        };
        i32 a;
    };

    struct
    {
        i32 ignore0;
        union
        {
            iv2 yz;
            iv3 yzw;
        };
    };

    struct
    {
        iv2 xy;
        union
        {
            iv2 zw;
            iv2 wh;
            struct
            {
                i32 width;
                i32 height;
            };
        };
    };

    i32 elements[4];
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

#define iv2(...)                                                                                   \
    (iv2)                                                                                          \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define iv3(...)                                                                                   \
    (iv3)                                                                                          \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define iv4(...)                                                                                   \
    (iv4)                                                                                          \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }

m4 M4_Identity(void);
m4 M4xM4(m4 a, m4 b);
m4 M4_Translate(m4 m, v3 translation);
m4 M4_Scale(m4 m, v3 scale);
