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

    f32 elements[2];
};

#define v2(x, y)                                                                         \
    (v2)                                                                                 \
    {                                                                                    \
        x, y                                                                             \
    }

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

    f32 elements[3];
};

#define v3(x, y, z)                                                                      \
    (v3)                                                                                 \
    {                                                                                    \
        x, y, z                                                                          \
    }

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
        v2 zw;
    };

    f32 elements[4];
};

#define v4(x, y, z, w)                                                                   \
    (v4)                                                                                 \
    {                                                                                    \
        x, y, z, w                                                                       \
    }

typedef union m4 m4;
union m4
{
    f32 columns[4][4];
    f32 elements[16];
};

#define m4(d)                                                                            \
    (m4)                                                                                 \
    {                                                                                    \
        {                                                                                \
            {d, 0, 0, 0}, {0, d, 0, 0}, {0, 0, d, 0}, {0, 0, 0, d},                      \
        }                                                                                \
    }

v2 V2Normalize(v2 v);

b32 RectVersusV2(v4 rect, v2 point);

m4 M4Translate(m4 m, v3 translate);
m4 M4Scale(m4 m, v3 scale);
m4 M4xM4(m4 a, m4 b);
v4 M4xV4(m4 m, v4 v);
m4 M4Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
