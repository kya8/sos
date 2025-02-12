#include "macros.h"
#include <sos.h>

int main(void)
{
    Sos s1, s2, s3, s4;
    sos_init(&s1);
    sos_init_with_cap(&s2, 32);
    sos_init_from_str(&s3, "Hello");
    sos_copy(&s4, &s3);

    ASSERT(sos_len(&s1) == 0);
    ASSERT(sos_len(&s2) == 0 && sos_cap(&s2) >= 32);
    ASSERT(sos_len(&s3) == 5 && sos_cap(&s3) >= 5);
    ASSERT(sos_eq(&s3, &s4));

    sos_reserve(&s2, 1024);
    ASSERT(sos_cap(&s2) >= 1024);

    Sos l1, l2;
    const char* const long_str = "woooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooorld!";
    sos_init_from_str(&l1, long_str);
    sos_init(&l2);
    sos_append(&l2, &s3);

    ASSERT(sos_eq_str(&l1, long_str));
    ASSERT(sos_eq(&l2, &s3));

    sos_swap(&l1, &l2);
    ASSERT(sos_eq_str(&l2, long_str));
    sos_swap(&l1, &l2);

    sos_push(&l2, ' ');
    sos_append_str(&l2, long_str);
    ASSERT_EQ(sos_pop(&l2), '!');
    ASSERT(sos_eq_str(&l2, "Hello woooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooorld"));

    const size_t l2_cap = sos_cap(&l2);
    sos_clear(&l2);
    ASSERT(sos_len(&l2) == 0);
    ASSERT_EQ(sos_cap(&l2), l2_cap);

    return 0;
}
