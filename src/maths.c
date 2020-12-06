internal v2 V2AddV2(v2 a, v2 b)
{
    v2 c = {a.x + b.x, a.y + b.y};
    return c;
}

internal v2 V2MinusV2(v2 a, v2 b)
{
    v2 c = {a.x - b.x, a.y - b.y};
    return c;
}

internal v2 V2MultiplyF32(v2 v, f32 f)
{
    v.x *= f;
    v.y *= f;
    return v;
}

internal f32 V2LengthSquared(v2 a)
{
    return a.x * a.x + a.y * a.y;
}

internal f32 V2Length(v2 a)
{
    return SquareRoot(V2LengthSquared(a));
}

internal v2 V2Normalize(v2 v)
{
    v2 result;
    f32 length = V2Length(v);

    if (length <= F32_EPSILON)
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

internal f32 V2Dot(v2 a, v2 b)
{
    return a.x * b.x + a.y * b.y;
}

internal v2 V2FromIV2(iv2 a)
{
    v2 result = {(f32)a.x, (f32)a.y};
    return result;
}

internal f32 PointDirection(v2 a, v2 b)
{
    f32 x = b.x - a.x;
    f32 y = b.y - a.y;
    return ATan2(y, x);
}

internal v3 V3AddV3(v3 a, v3 b)
{
    v3 c = {a.x + b.x, a.y + b.y, a.z + b.z};
    return c;
}

internal v3 V3MinusV3(v3 a, v3 b)
{
    v3 c = {a.x - b.x, a.y - b.y, a.z - b.z};
    return c;
}

internal v3 V3MultiplyF32(v3 v, f32 f)
{
    v.x *= f;
    v.y *= f;
    v.z *= f;
    return v;
}

internal f32 V3LengthSquared(v3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

internal f32 V3Length(v3 a)
{
    return SquareRoot(V3LengthSquared(a));
}

internal v3 V3Normalize(v3 v)
{
    v3 result;
    f32 length = V3Length(v);

    if (length <= F32_EPSILON)
    {
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }
    else
    {
        result.x = v.x / length;
        result.y = v.y / length;
        result.z = v.z / length;
    }

    return result;
}

internal f32 V3Dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

internal v3 V3Cross(v3 a, v3 b)
{
    v3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
    return result;
}

internal b32 V4HasPoint(v4 v, v2 p)
{
    return p.x >= v.x && p.x <= v.x + v.width && p.y >= v.y && p.y <= v.y + v.height;
}

internal f32 V4Dot(v4 a, v4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

internal v4 V4AddV4(v4 a, v4 b)
{
    v4 c = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return c;
}

internal v4 V4MinusV4(v4 a, v4 b)
{
    v4 c = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return c;
}

internal v4 V4MultiplyV4(v4 a, v4 b)
{
    v4 c = {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    return c;
}

internal v4 V4MultiplyM4(v4 v, m4 m)
{
    v4 result = {0};

    for (int i = 0; i < 4; i++)
    {
        result.elements[i] = (v.elements[0] * m.elements[0][i] + v.elements[1] * m.elements[1][i] +
                              v.elements[2] * m.elements[2][i] + v.elements[3] * m.elements[3][i]);
    }

    return result;
}

internal m4 M4Diagonal(f32 diagonal)
{
    m4 m = {{
        {diagonal},
        {0.0f, diagonal},
        {0.0f, 0.0f, diagonal},
        {0.0f, 0.0f, 0.0f, diagonal},
    }};
    return m;
}

internal m4 M4Identity()
{
    m4 m = {{
        {1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }};
    return m;
}

internal m4 M4MultiplyM4(m4 a, m4 b)
{
    m4 m = {0};

    for (int slow = 0; slow < 4; slow++)
    {
        for (int fast = 0; fast < 4; fast++)
        {
            m.elements[fast][slow] = (a.elements[0][slow] * b.elements[fast][0] +
                                      a.elements[1][slow] * b.elements[fast][1] +
                                      a.elements[2][slow] * b.elements[fast][2] +
                                      a.elements[3][slow] * b.elements[fast][3]);
        }
    }

    return m;
}

internal m4 M4MultiplyF32(m4 a, f32 b)
{
    for (int slow = 0; slow < 4; slow++)
    {
        for (int fast = 0; fast < 4; fast++)
        {
            a.elements[slow][fast] *= b;
        }
    }

    return a;
}

internal m4 M4Translate(v3 translation)
{
    m4 result = M4Identity();
    result.elements[3][0] = translation.x;
    result.elements[3][1] = translation.y;
    result.elements[3][2] = translation.z;
    return result;
}

internal m4 M4Scale(v3 scale)
{
    m4 result = M4Identity();
    result.elements[0][0] = scale.x;
    result.elements[1][1] = scale.y;
    result.elements[2][2] = scale.z;
    return result;
}

internal m4 M4RotateX(f32 angle)
{
    m4 result = M4Identity();
    f32 c = Cos(angle);
    f32 s = Sin(angle);

    result.elements[1][1] = c;
    result.elements[1][2] = -s;
    result.elements[2][1] = s;
    result.elements[2][2] = c;

    return result;
}

internal m4 M4RotateY(f32 angle)
{
    m4 result = M4Identity();
    f32 c = Cos(angle);
    f32 s = Sin(angle);

    result.elements[0][0] = c;
    result.elements[0][2] = s;
    result.elements[2][0] = -s;
    result.elements[2][2] = c;

    return result;
}

internal m4 M4RotateZ(f32 angle)
{
    m4 result = M4Identity();
    f32 c = Cos(angle);
    f32 s = Sin(angle);

    result.elements[0][0] = c;
    result.elements[0][1] = -s;
    result.elements[1][0] = s;
    result.elements[1][1] = c;

    return result;
}

internal m4 M4Rotate(f32 angle, v3 axis)
{
    m4 result = {0};
    f32 c = Cos(angle);
    f32 s = Sin(angle);

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

internal m4 M4Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
    m4 result = {0};

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

internal v3 RGBToHSV(v3 rgb)
{
    f32 r = rgb.r;
    f32 g = rgb.g;
    f32 b = rgb.b;
    f32 k = 0.f;
    if (g < b)
    {
        f32 swap = b;
        b = g;
        g = swap;
        k = -1.f;
    }
    if (r < g)
    {
        f32 swap = r;
        r = g;
        g = swap;
        k = -2.f / 6.f - k;
    }

    f32 chroma = r - (g < b ? g : b);
    v3 result;
    result.x = AbsoluteValue(k + (g - b) / (6.f * chroma + 1e-20f));
    result.y = chroma / (r + 1e-20f);
    result.z = r;
    return result;
}

internal v3 HSVToRGB(v3 hsv)
{
    v3 rgb = {0};

    if (hsv.y == 0.0f)
    {
        rgb.r = rgb.g = rgb.b = hsv.z;
    }
    else
    {
        f32 h = hsv.x;
        f32 s = hsv.y;
        f32 v = hsv.z;

        if (h >= 1.f)
        {
            h -= 10 * 1e-6f;
        }

        if (s >= 1.f)
        {
            s -= 10 * 1e-6f;
        }

        if (v >= 1.f)
        {
            v -= 10 * 1e-6f;
        }

        h = FMod(h, 1.f) / (60.f / 360.f);
        int i = (int)h;
        float f = h - (float)i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i)
        {
        case 0:
            rgb.r = v;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = v;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = v;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = v;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = v;
            break;
        case 5:
        default:
            rgb.r = v;
            rgb.g = p;
            rgb.b = q;
            break;
        }
    }

    return rgb;
}
