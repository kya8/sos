# Intro
`sos` is an implementation of **S**mall-buffer **O**ptimized **S**tring for C.

An `sos` string is able to store short strings inline within itself, and allocate additional memory as the string grows.
This reduces memory allocations especially when dealing with large amount of short strings.

# Implementation
This is achieved with a union of short and long representations. The union is tagged with a special bit, to distinguish `sos` strings in short and long mode at runtime. The idea is inspired by the `std::string` implementation in libc++.

On most 64-bit platforms, `sos` can hold short strings with length up to 22 chars (excluding the null character).

# Usage
Minimal example:
```c
Sos s1, s2;
sos_init_from_str(&s1, "Hello!");
sos_init_with_cap(&s2, 16);
sos_pop(&s1);
sos_append_str(&s2, ", world");
sos_append(&s1, &s2);
sos_push(&s1, '!');

printf("%s\n", sos_str(&s1));

sos_finish(&s2);
sos_finish(&s1);
```
See `sos.h` for more.

# TODO
* Configurable small buffer size
* Some missing checks for max length
