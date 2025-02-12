#include <stdio.h>
#include "sos.h"

int main(void)
{
    Sos s1, s2;
    sos_init_from_str(&s1, "Hello!");
    sos_init_with_cap(&s2, 16);

    sos_pop(&s1);
    sos_append_str(&s2, ", world");
    sos_append(&s1, &s2);
    sos_push(&s1, '!');

    printf("%s\nCap: %zu\n", sos_str(&s1), sos_cap(&s1));

    sos_finish(&s2);
    sos_finish(&s1);
}
