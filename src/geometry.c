#include "geometry.h"
#include <math.h>

v2 V2Normalize(v2 v)
{
    if (v.x == 0.0f && v.y == 0.0f)
    {
        return (v2){0, 0};
    }

    f32 len = sqrtf(v.x * v.x + v.y * v.y);
    return (v2){v.x / len, v.y / len};
}

m4 M4Translate(m4 m, v3 translate)
{
    m4 result = m;
    result.columns[3][0] += translate.x;
    result.columns[3][1] += translate.y;
    result.columns[3][2] += translate.z;
    return result;
}

m4 M4Scale(m4 m, v3 scale)
{
    m4 result = m;
    result.columns[0][0] *= scale.x;
    result.columns[1][1] *= scale.y;
    result.columns[2][2] *= scale.z;
    return result;
}

m4 M4xM4(m4 a, m4 b)
{
    m4 result;

    for (i32 c = 0; c < 4; ++c)
    {
        for (i32 r = 0; r < 4; ++r)
        {
            result.columns[c][r] = (a.columns[0][r] * b.columns[c][0]) +
                                   (a.columns[1][r] * b.columns[c][1]) +
                                   (a.columns[2][r] * b.columns[c][2]) +
                                   (a.columns[3][r] * b.columns[c][3]);
        }
    }

    return result;
}

v4 M4xV4(m4 m, v4 v)
{
    v4 result;

    for (i32 i = 0; i < 4; ++i)
    {
        result.elements[i] = (v.x * m.columns[0][i]) + (v.y * m.columns[1][i]) +
                             (v.z * m.columns[2][i]) + (v.w * m.columns[3][i]);
    }

    return result;
}

m4 M4Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
    m4 result = {0};

    f32 rl = 1.0f / (right - left);
    f32 tb = 1.0f / (top - bottom);
    f32 fn = -1.0f / (zFar - zNear);

    result.columns[0][0] = 2.0f * rl;
    result.columns[1][1] = 2.0f * tb;
    result.columns[2][2] = 2.0f * fn;
    result.columns[3][0] = -(right + left) * rl;
    result.columns[3][1] = -(top + bottom) * tb;
    result.columns[3][2] = (zFar + zNear) * fn;
    result.columns[3][3] = 1.0f;

    return result;
}
