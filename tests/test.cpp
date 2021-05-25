#include "test.hpp"

#include "span_test.hpp"

TEST(TestFramework, PassingTest)
{
    EXPECT(1 + 1 == 2);
}

TEST(TestFramework, FailingTest)
{
    EXPECT(1 + 1 != 2);
}

int main(void)
{
    return RunAllTests();
}
