#ifndef SOS_TEST_MACROS_H
#define SOS_TEST_MACROS_H

#include <stdlib.h>
#include <stdio.h>
#include <sos.h>

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

#define ASSERT_SOS_EQ(L, R) ASSERT(sos_cmp(&(L), &(R)) == 0)
#define ASSERT_SOS_EQS(L, R) ASSERT(sos_cmp_cstr(&(L), (R)) == 0)

#endif /* SOS_TEST_MACROS_H */
