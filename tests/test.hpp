#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#endif

static struct TestState
{
    struct TestCase
    {
        std::string_view name;
        void (*RunTest)(void);
    };

    struct TestCategory
    {
        std::string_view name;
        std::vector<TestCase> cases;
    };

    std::vector<TestCategory> m_Categories;
    std::string m_Reason;

#ifdef _WIN32
    HANDLE m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    void RegisterTestCase(std::string_view categoryName, std::string_view testName,
                          void (*TestFn)(void))
    {
        auto &it = std::find_if(m_Categories.begin(), m_Categories.end(),
                                [categoryName](const TestCategory &category)
                                { return categoryName == category.name; });

        TestCase testcase{testName, TestFn};
        if (it == m_Categories.end())
        {
            m_Categories.push_back({categoryName, {testcase}});
        }
        else
        {
            it->cases.push_back(testcase);
        }
    }
} s_TestState;

#define _INITIALIZER_T(f) f##_t_
#define _INITIALIZER_U(f) f##_
#define INITIALIZER(f)                                                                   \
    void f(void);                                                                        \
    static struct _INITIALIZER_T(f)                                                      \
    {                                                                                    \
        _INITIALIZER_T(f)(void)                                                          \
        {                                                                                \
            f();                                                                         \
        }                                                                                \
    } _INITIALIZER_U(f);                                                                 \
    void f(void)

#define TEST(category, testName)                                                         \
    void(_TestFn_##category##_##testName)(void);                                         \
    INITIALIZER(_TestFn_##category##_##testName##_Init)                                  \
    {                                                                                    \
        s_TestState.RegisterTestCase(#category, #testName,                               \
                                     _TestFn_##category##_##testName);                   \
    }                                                                                    \
    void(_TestFn_##category##_##testName)(void)

#define EXPECT(cond)                                                                     \
    if (!(cond))                                                                         \
    {                                                                                    \
        std::ostringstream ss;                                                           \
        ss << "EXPECT(" << #cond << ")";                                                 \
        s_TestState.m_Reason = ss.str();                                                 \
        return;                                                                          \
    }

static int RunAllTests(void)
{
    int testsRan = 0;
    int testsPassed = 0;

#ifdef _WIN32
    int COL_RESET = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    int FG_RED = FOREGROUND_RED;
    int FG_GREEN = FOREGROUND_GREEN;

    auto ConsoleSetColor = [](int color) -> void
    { SetConsoleTextAttribute(s_TestState.m_ConsoleHandle, color); };
#else
    std::string_view COL_RESET = "\x1b[0m";
    std::string_view FG_RED = "\x1b[31m";
    std::string_view FG_GREEN = "\x1b[32m";

    auto ConsoleSetColor = [](std::string_view color) -> void
    { std::cout << color };
#endif

    for (const auto &category : s_TestState.m_Categories)
    {
        for (const auto &testcase : category.cases)
        {
            testcase.RunTest();
            testsRan++;

            if (s_TestState.m_Reason == "")
            {
                ConsoleSetColor(FG_GREEN);
                std::cout << "PASS";
                ConsoleSetColor(COL_RESET);
                std::cout << " " << category.name << ", " << testcase.name << "\n";
                testsPassed++;
            }
            else
            {
                ConsoleSetColor(FG_RED);
                std::cout << "FAIL";
                ConsoleSetColor(COL_RESET);
                std::cout << " " << category.name << ", " << testcase.name << ": "
                          << s_TestState.m_Reason << "\n";
                s_TestState.m_Reason = "";
            }
        }
    }

    std::cout << testsPassed << " out of " << testsRan << " tests passed.\n";

    return testsRan != testsPassed;
}
