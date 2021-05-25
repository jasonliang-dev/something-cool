#include "../src/geometry.hpp"
#include <climits>
#include <cstring>

TEST(Matrix4x4, Zero)
{
    m4 m = m4(0);

    for (float f : m.elements)
    {
        EXPECT(f == 0.0f);
    }
}

TEST(Matrix4x4, Identity)
{
    m4 m = m4(1);

    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            if (r == c)
            {
                EXPECT(m[c][r] == 1.0f);
            }
            else
            {
                EXPECT(m[c][r] == 0.0f);
            }
        }
    }
}

TEST(Matrix4x4, Translate)
{
    m4 m = m4(1).Translate(v3{1, 2, 3});

    v4 result = m * v4{1.0f, 2.0f, 3.0f, 1.0f};
    // I'm laughing.
    EXPECT(result.x == 2.0f);
    EXPECT(result.y == 4.0f);
    EXPECT(result.z == 6.0f);
    EXPECT(result.w == 1.0f);
}

TEST(Matrix4x4, Scale)
{
    m4 m = m4(1).Scale(v3{3, 3, 3});

    v4 result = m * v4{1.0f, 2.0f, 3.0f, 1.0f};
    EXPECT(result.x == 3.0f);
    EXPECT(result.y == 6.0f);
    EXPECT(result.z == 9.0f);
    EXPECT(result.w == 1.0f);
}

TEST(Matrix4x4, TranslationAndScale)
{
    m4 m = m4(1).Translate(v3{1, 2, 3}).Scale(v3{2, 2, 2});

    v4 result = m * v4{1.0f, 2.0f, 3.0f, 1.0f};
    EXPECT(result.x == 3.0f);
    EXPECT(result.y == 6.0f);
    EXPECT(result.z == 9.0f);
    EXPECT(result.w == 1.0f);
}
