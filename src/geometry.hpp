#pragma once

union v2
{
    struct
    {
        float x;
        float y;
    };

    float elements[2];

    float &operator[](size_t i)
    {
        return elements[i];
    }
};

union v3
{
    struct
    {
        float x;
        float y;
        float z;
    };

    v2 xy;

    struct
    {
        float ignore0;
        v2 yz;
    };

    float elements[3];

    float &operator[](size_t i)
    {
        return elements[i];
    }
};

union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                float x;
                float y;
                float z;
            };
        };
        float w;
    };

    struct
    {
        float ignore0;
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

    float elements[4];

    float &operator[](size_t i)
    {
        return elements[i];
    }
};

union m4
{
    v4 columns[4];
    float elements[16] = {};

    m4(void) = default;

    m4(float f)
    {
        columns[0][0] = f;
        columns[1][1] = f;
        columns[2][2] = f;
        columns[3][3] = f;
    }

    m4 &Translate(v3 translate)
    {
        columns[3][0] += translate.x;
        columns[3][1] += translate.y;
        columns[3][2] += translate.z;
        return *this;
    }

    m4 &Scale(v3 scale)
    {
        columns[0][0] *= scale.x;
        columns[1][1] *= scale.y;
        columns[2][2] *= scale.z;
        return *this;
    }

    v4 &operator[](size_t i)
    {
        return columns[i];
    }

    static m4 Orthographic(float left, float right, float bottom, float top, float zNear,
                           float zFar)
    {
        m4 result{};

        float rl = 1.0f / (right - left);
        float tb = 1.0f / (top - bottom);
        float fn = -1.0f / (zFar - zNear);

        result[0][0] = 2.0f * rl;
        result[1][1] = 2.0f * tb;
        result[2][2] = 2.0f * fn;
        result[3][0] = -(right + left) * rl;
        result[3][1] = -(top + bottom) * tb;
        result[3][2] = (zFar + zNear) * fn;
        result[3][3] = 1.0f;

        return result;
    }
};

static m4 operator*(m4 a, m4 b)
{
    m4 result;

    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            result[c][r] = (a[0][r] * b[c][0]) + (a[1][r] * b[c][1]) +
                           (a[2][r] * b[c][2]) + (a[3][r] * b[c][3]);
        }
    }

    return result;
}

static v4 operator*(m4 m, v4 v)
{
    v4 result{};

    for (int i = 0; i < 4; ++i)
    {
        result.elements[i] =
            (v.x * m[0][i]) + (v.y * m[1][i]) + (v.z * m[2][i]) + (v.w * m[3][i]);
    }

    return result;
}
