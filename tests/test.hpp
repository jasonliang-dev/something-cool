#pragma once

#include <iostream>
#include <sstream>
#include <vector>

static struct
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

    void RegisterTestCase(std::string_view categoryName, std::string_view testName,
                          void (*TestFn)(void))
    {
        auto &it = std::find_if(m_Categories.begin(), m_Categories.end(),
                                [categoryName](const TestCategory &category)
                                { return categoryName == category.name; });

        TestCase test{testName, TestFn};
        if (it == m_Categories.end())
        {
            m_Categories.push_back({categoryName, {test}});
        }
        else
        {
            it->cases.push_back(test);
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

    for (const auto &category : s_TestState.m_Categories)
    {
        for (const auto &testcase : category.cases)
        {
            testcase.RunTest();

            if (s_TestState.m_Reason == "")
            {
                std::cout << "[PASS] " << category.name << ", " << testcase.name << "\n";
                testsPassed++;
            }
            else
            {
                std::cout << "[FAIL] " << category.name << ", " << testcase.name << ": "
                          << s_TestState.m_Reason << "\n";
                s_TestState.m_Reason = "";
            }

            testsRan++;
        }
    }

    std::cout << testsPassed << " out of " << testsRan << " tests passed.\n";

    return testsRan != testsPassed;
}
