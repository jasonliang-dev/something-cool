#include "../src/span.hpp"
#include <vector>

TEST(Span, ConstructDestruct)
{
    Span<int> span;
    (void)span;
}

TEST(Span, OperatorSquareBrackets)
{
    std::vector<int> nums{1, 2, 3, 4, 5};
    Span<int> span(nums.data(), 3);

    EXPECT(span[0] == 1);
    EXPECT(span[1] == 2);
    EXPECT(span[2] == 3);
}

TEST(Span, RangeFor)
{
    std::vector<int> nums{1, 2, 3, 4, 5};
    Span<int> span(nums.data(), 3);

    int i = 1;
    for (int n : span)
    {
        EXPECT(n == i);
        i++;
    }
}
