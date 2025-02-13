#include "macros.h"
#include <sos.h>

void test_simple(void)
{
    Sos s1, s2, s3, s4;
    sos_init(&s1);
    sos_init_with_cap(&s2, 32);
    sos_init_from_cstr(&s3, "Hello");
    sos_init_by_copy(&s4, &s3);

    ASSERT(sos_len(&s1) == 0);
    ASSERT(sos_len(&s2) == 0 && sos_cap(&s2) >= 32);
    ASSERT(sos_len(&s3) == 5 && sos_cap(&s3) >= 5);
    ASSERT_SOS_EQ(s3, s4);

    sos_reserve(&s2, 1024);
    ASSERT(sos_cap(&s2) >= 1024);

    Sos l1, l2;
    const char* const long_str = "woooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooorld!";
    sos_init_from_cstr(&l1, long_str);
    sos_init(&l2);
    sos_append(&l2, &s3);

    ASSERT_SOS_EQS(l1, long_str);
    ASSERT_SOS_EQ(l2, s3);

    sos_swap(&l1, &l2);
    ASSERT_SOS_EQS(l2, long_str);
    sos_swap(&l1, &l2);

    sos_push(&l2, ' ');
    sos_append_cstr(&l2, long_str);
    ASSERT_EQ(sos_pop(&l2), '!');
    ASSERT_SOS_EQS(l2, "Hello woooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooorld");

    sos_resize(&l1, 3, '?');
    ASSERT_SOS_EQS(l1, "woo");
    sos_resize(&l1, 5, 'o');
    ASSERT_SOS_EQS(l1, "woooo");

    const size_t l2_cap = sos_cap(&l2);
    sos_clear(&l2);
    ASSERT(sos_len(&l2) == 0);
    ASSERT_EQ(sos_cap(&l2), l2_cap);

    sos_finish(&s1);
    sos_finish(&s2);
    sos_finish(&s3);
    sos_finish(&s4);
    sos_finish(&l1);
    sos_finish(&l2);
}

void test_cmp(void)
{
    Sos s1, s2, s3;
    sos_init_from_cstr(&s1, "abc");
    sos_init_from_cstr(&s2, "abcdefghijklmnopqrstuvwxyz0123456789");
    sos_init_by_copy(&s3, &s2);

    ASSERT(sos_cmp(&s1, &s2) < 0);
    ASSERT(sos_cmp(&s2, &s3) == 0);
    ASSERT(sos_cmp_cstr(&s3, "abcdefghijklmnopqrstuvwxyz0023456789") > 0);

    sos_finish(&s1);
    sos_finish(&s2);
    sos_finish(&s3);
}

void test_fmt(void)
{
    Sos s1, s2;
    const char* name = "Bob";
    sos_init_format(&s1, "Hello, %s!", name);
    sos_init_format(&s2, "There are %d letters in the word %s.", 45, "pneumonoultramicroscopicsilicovolcanoconiosis");
    ASSERT_SOS_EQS(s1, "Hello, Bob!");
    ASSERT_SOS_EQS(s2, "There are 45 letters in the word pneumonoultramicroscopicsilicovolcanoconiosis.");

    sos_finish(&s2);
    sos_finish(&s1);
}

int main(void)
{
    test_cmp();
    test_simple();
    test_fmt();

    return 0;
}
