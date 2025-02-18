#include "common.h"

int fmt(int argc, char** argv)
{
    (void)argc; (void)argv;

    Sos s1, s2;
    const char* name = "Bob";
    sos_init_format(&s1, "Hello, %s!", name);
    sos_init_format(&s2, "There are %d letters in the word %s.", 45, "pneumonoultramicroscopicsilicovolcanoconiosis");
    ASSERT_SOS_EQS(s1, "Hello, Bob!");
    ASSERT_SOS_EQS(s2, "There are 45 letters in the word pneumonoultramicroscopicsilicovolcanoconiosis.");

    sos_finish(&s2);
    sos_finish(&s1);

    return 0;
}
