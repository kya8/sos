#include "macros.h"
#include <string.h>

int len(int argc, char** argv)
{
    (void)argc; (void)argv;

    Sos s;
    sos_init(&s);
    ASSERT(sos_len(&s) == 0);

    sos_resize(&s, 5, 'x');
    ASSERT(sos_len(&s) == 5);
    ASSERT_SOS_EQS(s, "xxxxx");

    sos_resize(&s, 3, '?');
    ASSERT(sos_len(&s) == 3);
    ASSERT_SOS_EQS(s, "xxx");

    Sos s_copy;
    sos_init_by_copy(&s_copy, &s);
    ASSERT(sos_len(&s_copy) == 3);
    ASSERT_SOS_EQ(s_copy, s);

    sos_resize(&s, 100, 'o');
    ASSERT(sos_len(&s) == 100);
    for (size_t i = 3; i < 100; ++i) {
        sos_push(&s_copy, 'o');
    }
    ASSERT_SOS_EQ(s_copy, s);

    sos_finish(&s_copy);

    sos_resize(&s, 3, 0);
    ASSERT(sos_len(&s) == 3);
    ASSERT_SOS_EQS(s, "xxx");

    const SosStatusAndBuf expand_ret = sos_expand_for_overwrite(&s, 6);
    ASSERT(expand_ret.status == SOS_OK);
    ASSERT(expand_ret.str == sos_cstr_mut(&s) + 3);
    ASSERT(sos_len(&s) == 9);

    sos_finish(&s);

    sos_init_with_cap(&s, 32);
    ASSERT(sos_len(&s) == 0);
    ASSERT(sos_cap(&s) >= 32);

    sos_finish(&s);

    const char* const long_str = "01234567890123456789012345678901234567890123456789";
    const size_t long_str_len = strlen(long_str);
    sos_init_from_range(&s, long_str, long_str_len);
    ASSERT(sos_len(&s) == long_str_len);
    ASSERT_SOS_EQS(s, long_str);

    sos_finish(&s);

    return 0;
}
