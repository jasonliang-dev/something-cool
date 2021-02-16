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

internal inline v2 operator*(v2 left, f32 right)
{
    return {left.x * right, left.y * right};
}

internal inline v2 operator/(v2 left, f32 right)
{
    return {left.x / right, left.y / right};
}

internal inline v2 &operator/=(v2 &left, f32 right)
{
    return (left = left / right);
}

internal inline v2 operator+(v2 left, v2 right)
{
    return {left.x + right.x, left.y + right.y};
}

internal inline v2 operator-(v2 left, v2 right)
{
    return {left.x - right.x, left.y - right.y};
}

internal inline v2 operator*(v2 left, v2 right)
{
    return {left.x * right.x, left.y * right.y};
}

internal inline v2 operator/(v2 left, v2 right)
{
    return {left.x / right.x, left.y / right.y};
}

internal inline v2 &operator+=(v2 &left, v2 right)
{
    return (left = left + right);
}

internal inline v2 &operator-=(v2 &left, v2 right)
{
    return (left = left - right);
}

internal inline v2 &operator/=(v2 &left, v2 right)
{
    return (left = left / right);
}
