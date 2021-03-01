internal inline f32 Signf(f32 f)
{
    return (f32)((f > 0) - (f < 0));
}

internal inline f32 Clampf(f32 f, f32 min, f32 max)
{
    f = f < min ? min : f;
    return f > max ? max : f;
}

internal inline v2 V2Normalize(v2 v)
{
    v2 result;
    f32 length = sqrtf(v.x * v.x + v.y * v.y);

    if (length <= FLT_EPSILON)
    {
        result.x = 0.0f;
        result.y = 0.0f;
    }
    else
    {
        result.x = v.x / length;
        result.y = v.y / length;
    }

    return result;
}

internal inline v2 V2Clamp(v2 v, v2 min, v2 max)
{
    v2 result;
    result.x = Clampf(v.x, min.x, max.x);
    result.y = Clampf(v.y, min.y, max.y);
    return result;
}

internal inline m4 M4Diagonal(f32 diagonal)
{
    m4 result = {{
        {diagonal},
        {0.0f, diagonal},
        {0.0f, 0.0f, diagonal},
        {0.0f, 0.0f, 0.0f, diagonal},
    }};
    return result;
}

internal inline m4 M4Translate(v3 translation)
{
    m4 result = M4Diagonal(1);
    result.elements[3][0] = translation.x;
    result.elements[3][1] = translation.y;
    result.elements[3][2] = translation.z;
    return result;
}

internal inline m4 M4Scale(v3 scale)
{
    m4 result = M4Diagonal(1);
    result.elements[0][0] = scale.x;
    result.elements[1][1] = scale.y;
    result.elements[2][2] = scale.z;
    return result;
}

internal inline m4 M4RotateX(f32 angle)
{
    m4 result = M4Diagonal(1);
    f32 c = cosf(angle);
    f32 s = sinf(angle);

    result.elements[1][1] = c;
    result.elements[1][2] = -s;
    result.elements[2][1] = s;
    result.elements[2][2] = c;

    return result;
}

internal inline m4 M4RotateY(f32 angle)
{
    m4 result = M4Diagonal(1);
    f32 c = cosf(angle);
    f32 s = sinf(angle);

    result.elements[0][0] = c;
    result.elements[0][2] = s;
    result.elements[2][0] = -s;
    result.elements[2][2] = c;

    return result;
}

internal inline m4 M4RotateZ(f32 angle)
{
    m4 result = M4Diagonal(1);
    f32 c = cosf(angle);
    f32 s = sinf(angle);

    result.elements[0][0] = c;
    result.elements[0][1] = -s;
    result.elements[1][0] = s;
    result.elements[1][1] = c;

    return result;
}

internal inline m4 M4Rotate(f32 angle, v3 axis)
{
    m4 result = M4Diagonal(0);
    f32 c = cosf(angle);
    f32 s = sinf(angle);

    result.elements[0][0] = c + (axis.x * axis.x) * (1 - c);
    result.elements[0][1] = (axis.x * axis.y) * (1 - c) - (axis.z * s);
    result.elements[0][2] = (axis.x * axis.z) * (1 - c) + (axis.y * s);

    result.elements[1][0] = (axis.y * axis.x) * (1 - c) + (axis.z * s);
    result.elements[1][1] = c + (axis.y * axis.y) * (1 - c);
    result.elements[1][2] = (axis.y * axis.z) * (1 - c) - (axis.x * s);

    result.elements[2][0] = (axis.z * axis.x) * (1 - c) - (axis.y * s);
    result.elements[2][1] = (axis.z * axis.y) * (1 - c) + (axis.x * s);
    result.elements[2][2] = c + (axis.z * axis.z) * (1 - c);

    result.elements[3][3] = 1;

    return result;
}

internal inline m4 M4Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
    m4 result = M4Diagonal(0);

    f32 rl = 1.0f / (right - left);
    f32 tb = 1.0f / (top - bottom);
    f32 fn = -1.0f / (zFar - zNear);

    result.elements[0][0] = 2.0f * rl;
    result.elements[1][1] = 2.0f * tb;
    result.elements[2][2] = 2.0f * fn;
    result.elements[3][0] = -(right + left) * rl;
    result.elements[3][1] = -(top + bottom) * tb;
    result.elements[3][2] = (zFar + zNear) * fn;
    result.elements[3][3] = 1.0f;

    return result;
}

internal inline m4 M4MultiplyM4(m4 a, m4 b)
{
    m4 result = M4Diagonal(0);

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            result.elements[x][y] =
                (a.elements[0][y] * b.elements[x][0] + a.elements[1][y] * b.elements[x][1] +
                 a.elements[2][y] * b.elements[x][2] + a.elements[3][y] * b.elements[x][3]);
        }
    }

    return result;
}

internal inline v4 V4MultiplyM4(v4 v, m4 m)
{
    v4 result;

    for (int i = 0; i < 4; ++i)
    {
        result.elements[i] = (v.x * m.elements[0][i] + v.y * m.elements[1][i] +
                              v.z * m.elements[2][i] + v.w * m.elements[3][i]);
    }

    return result;
}
