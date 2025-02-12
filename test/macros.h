#ifndef SOS_TEST_MACROS_H
#define SOS_TEST_MACROS_H

#include <stdlib.h>
#include <stdio.h>

#define QUOTE(str) #str
#define STR(str) QUOTE(str)

#define ASSERT(COND)                                                                                              \
    do {                                                                                                          \
        if (!(COND)) {                                                                                            \
            fprintf(stderr, "%s:%s: %s: Assertion `%s' failed!\n", __FILE__, STR(__LINE__), __func__, STR(COND)); \
            exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                         \
    } while (0)

#define ASSERT_EQ(L, R) ASSERT((L) == (R))

#endif /* SOS_TEST_MACROS_H */
