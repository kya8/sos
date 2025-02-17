# Intro
`sos` is an implementation of **S**mall-buffer **O**ptimized **S**tring for C.

An `sos` string is able to store short strings inline within itself, and allocate additional memory as the string grows.
This reduces dynamic memory allocations especially when dealing with large amount of short strings.

# Implementation
The small buffer optimization is achieved with a union of short and long string representations.
The union is tagged with a special bit, to distinguish `sos` strings in short / long mode at runtime.
The idea is inspired by the `std::string` implementation in libc++.

The internal string buffer is always null-terminated like `std::string`, so `sos` can be used as C string at no cost.

On typical 64-bit platforms, `sos` can hold short strings with length up to 22 chars (excluding the null character).

# Usage
Minimal example:
```c
Sos s1, s2;
sos_init_from_cstr(&s1, "Hello!");
sos_init_with_cap(&s2, 16);
sos_pop(&s1);
sos_append_cstr(&s2, ", world");
sos_append(&s1, &s2);
sos_push(&s1, '!');

printf("%s\n", sos_cstr(&s1));

Sos s3;
sos_init_format(&s3, "The answer is %d", 42);

sos_finish(&s3);
sos_finish(&s2);
sos_finish(&s1);
```
See `sos.h` for more.

# TODO
* Configurable small buffer size
* Some missing checks for max length
* Test coverage
