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
    f32 length = V2Length(v);
    v2 result = {
        v.x / length,
        v.y / length,
    };
    return result;
}

internal f32 V2Dot(v2 a, v2 b)
{
    return a.x * b.x + a.y * b.y;
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
    f32 length = V3Length(v);
    v3 result = {
        v.x / length,
        v.y / length,
        v.z / length,
    };
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
