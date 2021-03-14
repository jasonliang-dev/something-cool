#pragma once

#include <assert.h>
#include <stdio.h>

#ifdef _WIN32
#    include <windows.h>
#endif

typedef struct TestCase TestCase;
struct TestCase
{
    const char *name;
    void (*run)(const char **);
    TestCase *next;
};

typedef struct TestCategory TestCategory;
struct TestCategory
{
    const char *name;
    TestCase *cases;
    TestCategory *next;
};

typedef struct TestState TestState;
struct TestState
{
    int ran;
    int passed;
    const char *reason;
    TestCategory *head;
};

static TestState globalTestState;

#ifdef _MSC_VER
#    pragma section(".CRT$XCU", read)
#    define INITIALIZER2_(f, p)                                                                    \
        static void f(void);                                                                       \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f;                                   \
        __pragma(comment(linker, "/include:" p #f "_")) static void f(void)
#    ifdef _WIN64
#        define INITIALIZER(f) INITIALIZER2_(f, "")
#    else
#        define INITIALIZER(f) INITIALIZER2_(f, "_")
#    endif
#else
#    define INITIALIZER(f)                                                                         \
        static void f(void) __attribute__((constructor));                                          \
        static void f(void)
#endif

#define TEST(categoryName, caseName)                                                               \
    void(testFn##categoryName##caseName)(const char **);                                           \
    INITIALIZER(testFn##categoryName##caseName##init)                                              \
    {                                                                                              \
        RegisterTestCase(#categoryName, #caseName, testFn##categoryName##caseName);                \
    }                                                                                              \
    void(testFn##categoryName##caseName)(const char **testReason)

#define EXPECT(cond)                                                                               \
    if (!(cond))                                                                                   \
    {                                                                                              \
        *testReason = #cond;                                                                       \
        return;                                                                                    \
    }

static void RegisterTestCase(const char *categoryName, const char *caseName,
                             void (*run)(const char **))
{
    TestCategory *needle = NULL;
    for (TestCategory *cat = globalTestState.head; cat; cat = cat->next)
    {
        if (strcmp(cat->name, categoryName) == 0)
        {
            needle = cat;
            goto gotNeedle;
        }
    }

    needle = malloc(sizeof(TestCategory));
    needle->name = categoryName;
    needle->cases = NULL;
    needle->next = globalTestState.head;
    globalTestState.head = needle;

gotNeedle:;
    TestCase *newCase = malloc(sizeof(TestCase));
    newCase->name = caseName;
    newCase->run = run;
    newCase->next = needle->cases;
    needle->cases = newCase;
}

#define COLOR_RED(str) "\033[41m\033[30m" str "\033[0m"
#define COLOR_GREEN(str) "\033[42m\033[30m" str "\033[0m"

static void RunCase(TestCase *testCase)
{
    if (!testCase)
    {
        return;
    }

    RunCase(testCase->next);

    testCase->run(&globalTestState.reason);

    printf("  ");
    if (globalTestState.reason)
    {
        printf(COLOR_RED(" FAIL ") " %s : %s\n", testCase->name, globalTestState.reason);
    }
    else
    {
        printf(COLOR_GREEN(" PASS ") " %s\n", testCase->name);
        globalTestState.passed++;
    }

    globalTestState.ran++;

    free(testCase);
}

static void RunCategory(TestCategory *category)
{
    if (!category)
    {
        return;
    }

    RunCategory(category->next);

    printf("%s\n", category->name);
    RunCase(category->cases);

    free(category);
}

static int RunAllTests(void)
{
#ifdef _WIN32
    DWORD mode = 0;
    HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (stdoutHandle != INVALID_HANDLE_VALUE && GetConsoleMode(stdoutHandle, &mode))
    {
        SetConsoleMode(stdoutHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif

    globalTestState.ran = 0;
    globalTestState.passed = 0;

    RunCategory(globalTestState.head);
    printf("\n%d out of %d tests passed\n", globalTestState.passed, globalTestState.ran);

#ifdef _WIN32
    SetConsoleMode(stdoutHandle, mode);
#endif

    return globalTestState.ran != globalTestState.passed;
}
