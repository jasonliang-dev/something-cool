#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "language.h"
#include "strings.h"

#include "memory.c"
#include "strings.c"

TEST(String, CreateString)
{
    String s = CreateString("Hello World!");
    EXPECT(strcmp(s.ptr, "Hello World!") == 0);
    EXPECT(s.length == 12);
    FreeString(s);
}

TEST(String, CreateStringReserve)
{
    String s = CreateStringReserve(10);
    EXPECT(s.length == 0);
    EXPECT(s.ptr[0] == 0);
    FreeString(s);
}

TEST(String, Concat)
{
    String s = CreateString("foo");
    EXPECT(strcmp(StringCat(&s, "bar"), "foobar") == 0);
    EXPECT(strcmp(s.ptr, "foobar") == 0);
    FreeString(s);
}

TEST(String, ConcatMultiple)
{
    String s = CreateStringReserve(1);

    char buf[2] = {' ', 0};
    for (i32 i = 0; i < 1000; ++i)
    {
        buf[0] = i % 126 + 1;
        StringCat(&s, buf);
    }

    EXPECT(strstr(s.ptr, "ABCDEFGHIJKLMNOPQRSTUVWXYZ") != NULL);
    EXPECT(strstr(s.ptr, "abcdefghijklmnopqrstuvwxyz") != NULL);
    EXPECT(s.length == 1000);

    FreeString(s);
}

TEST(String, Remove)
{
    String s = CreateString("foobar");
    EXPECT(strcmp(StringRem(&s, 3), "foo") == 0);
    EXPECT(strcmp(s.ptr, "foo") == 0);
    FreeString(s);
}

TEST(String, RemoveOutOfBounds)
{
    String s = CreateString("foobar");
    EXPECT(StringRem(&s, INT_MAX)[0] == 0);
    FreeString(s);
}

TEST(GitHubWorkflows, FailingTest)
{
    EXPECT(false);
}

int main(void)
{
    return RunAllTests();
}
