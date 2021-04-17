#include "geometry.h"

m4 M4_Identity(void)
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

m4 M4_Translate(m4 m, v3 translation)
{
    m.elements[3][0] += translation.x;
    m.elements[3][1] += translation.y;
    m.elements[3][2] += translation.z;
    return m;
}

m4 M4_Scale(m4 m, v3 scale)
{
    m.elements[0][0] += scale.x;
    m.elements[1][1] += scale.y;
    m.elements[2][2] += scale.z;
    return m;
}
