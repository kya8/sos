#include "macros.h"

int cmp(int argc, char** argv)
{
    (void)argc; (void)argv;

    Sos s1, s2, s3;
    sos_init_from_cstr(&s1, "abc");
    sos_init_from_cstr(&s2, "abcdefghijklmnopqrstuvwxyz0123456789");
    sos_init_by_copy(&s3, &s2);

    ASSERT(sos_cmp(&s1, &s2) < 0);
    ASSERT(sos_cmp(&s2, &s3) == 0);
    ASSERT(sos_cmp_cstr(&s3, "abcdefghijklmnopqrstuvwxyz0023456789") > 0);

    ASSERT(sos_eq(&s2, &s3));
    ASSERT(sos_eq(&s1, &s1));
    ASSERT(!sos_eq(&s1, &s2));

    sos_finish(&s1);
    sos_finish(&s2);
    sos_finish(&s3);

    return 0;
}
