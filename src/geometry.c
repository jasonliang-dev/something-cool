#include "geometry.h"
#include <math.h>

v3 V3PlusV3(v3 a, v3 b)
{
    return v3(a.x + b.x, a.y + b.y, a.z + b.z);
}

v3 V3MinusV3(v3 a, v3 b)
{
    return v3(a.x - b.x, a.y - b.y, a.z - b.z);
}

v3 V3xV3(v3 a, v3 b)
{
    return v3(a.x * b.x, a.y * b.y, a.z * b.z);
}

v3 V3xF32(v3 v, f32 f)
{
    return v3(v.x * f, v.y * f, v.z * f);
}

v3 V3Normalize(v3 v)
{
    f32 length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return v3(v.x / length, v.y / length, v.z / length);
}

v3 V3Cross(v3 a, v3 b)
{
    f32 x = a.y * b.z - a.z * b.y;
    f32 y = a.z * b.x - a.x * b.z;
    f32 z = a.x * b.y - a.y * b.x;
    return v3(x, y, z);
}

f32 V3Dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

b32 V4EqualV4(v4 a, v4 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

m4 M4Identity(void)
{
    return (m4){{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }};
}

m4 M4xM4(m4 a, m4 b)
{
    m4 result;

    for (i32 y = 0; y < 4; ++y)
    {
        for (i32 x = 0; x < 4; ++x)
        {
            result.elements[x][y] =
                (a.elements[0][y] * b.elements[x][0] + a.elements[1][y] * b.elements[x][1] +
                 a.elements[2][y] * b.elements[x][2] + a.elements[3][y] * b.elements[x][3]);
        }
    }

    return result;
}

m4 M4Translate(m4 m, v3 translation)
{
    m.elements[3][0] += translation.x;
    m.elements[3][1] += translation.y;
    m.elements[3][2] += translation.z;
    return m;
}

m4 M4Scale(m4 m, v3 scale)
{
    m.elements[0][0] += scale.x;
    m.elements[1][1] += scale.y;
    m.elements[2][2] += scale.z;
    return m;
}
