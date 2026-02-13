#include "macros.h"
#include <stdio.h>

#define TEST_FMT(...)                                            \
    do {                                                         \
        Sos       s;                                             \
        char      buf[1024];                                     \
        const int ret = snprintf(buf, sizeof(buf), __VA_ARGS__); \
        ASSERT((ret >= 0 && (size_t)ret < sizeof(buf)));         \
        sos_init_format(&s, __VA_ARGS__);                        \
        ASSERT_SOS_EQS(s, buf);                                  \
        sos_finish(&s);                                          \
    } while (0)

int fmt(int argc, char** argv)
{
    (void)argc; (void)argv;

    TEST_FMT("Hello, world!");
    TEST_FMT("Hello, %s!\n", "Alice");
    TEST_FMT("The answer is %d.", 42);
    TEST_FMT("Pi is approximately %.2f.", 3.14159);
    TEST_FMT("This is a long string: %s",
             "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
             "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
             "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
             "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

    return 0;
}
