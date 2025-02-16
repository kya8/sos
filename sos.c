#include "sos.h"
#include <stdlib.h> // alloc
#include <string.h> // memcpy, strlen, strcmp
#include <stdint.h> // SIZE_MAX
#include <stdio.h>  // vsnprintf
#include <assert.h>
#include <stdarg.h>

// TODO: check for cap/size

#define SOS_MAX_LEN (SIZE_MAX - 2) // buffer len = str cap + 1, plus cap must be odd
#define SOS_MAX_LEN_FOR_EXPAND ((SOS_MAX_LEN - 1) / 2)

static int
is_long(const Sos* self)
{
    return (self->repr.s.len & 1u) == 1;
}

/**
 * Get length of short string
 *
 * @pre `self` is in short mode
 */
static unsigned char
short_len(const Sos* self)
{
    return self->repr.s.len >> 1;
}

/**
 * Set length of short string
 * 
 * @pre `self` is in short mode; `len` <= UCHAR_MAX >> 1
 */
static void
set_short_len(Sos* self, size_t len)
{
    self->repr.s.len = (unsigned char)(len << 1);
}

size_t sos_len(const Sos* self)
{
    if (is_long(self)) {
        return self->repr.l.len;
    }
    return short_len(self);
}

size_t sos_cap(const Sos* self)
{
    if (is_long(self)) {
        return self->repr.l.cap; // The string capacity is always odd in long mode
    }
    return SOS_SBO_BUFSIZE - 1;
}

const char* sos_cstr(const Sos* self)
{
    return is_long(self) ? self->repr.l.data : self->repr.s.data;
}

char* sos_cstr_mut(Sos* self)
{
    return (char*)sos_cstr(self);
}

SosView sos_view(const Sos* self)
{
    if (is_long(self)) {
        return (SosView) {.data = self->repr.l.data, .len = self->repr.l.len};
    }
    return (SosView) {.data = self->repr.s.data, .len = short_len(self)};
}

SosViewMut sos_view_mut(Sos* self)
{
    if (is_long(self)) {
        return (SosViewMut) {.data = self->repr.l.data, .len = self->repr.l.len};
    }
    return (SosViewMut) {.data = self->repr.s.data, .len = short_len(self)};
}

void sos_init(Sos* self)
{
    // We could not check here for unreleased self, since self can be uninitialized.
    self->repr.s.len = 0;
    self->repr.s.data[0] = 0;
}

SosStatus sos_init_with_cap(Sos* self, size_t cap)
{
    if (cap + 1 > SOS_SBO_BUFSIZE) { // long
        cap |= 1u; // add 1 if cap is even
        self->repr.l.data = malloc(cap + 1);
        if (!self->repr.l.data) {
            return SOS_ERROR_ALLOC;
        }
        self->repr.l.data[0] = 0;
        self->repr.l.len = 0;
        self->repr.l.cap = cap;
    } else { // short
        sos_init(self);
    }

    return SOS_OK;
}

SosStatusAndBuf sos_init_for_overwrite(Sos* self, size_t len)
{
    SosStatusAndBuf ret;

    if (len + 1 > SOS_SBO_BUFSIZE) {
        const size_t cap = len | 1u;
        self->repr.l.data = malloc(cap + 1);
        if (!self->repr.l.data) {
            ret.status = SOS_ERROR_ALLOC;
            return ret;
        }
        self->repr.l.data[len] = 0;
        self->repr.l.len = len;
        self->repr.l.cap = cap;
        ret.str = self->repr.l.data;
    } else {
        set_short_len(self, len);
        self->repr.s.data[len] = 0;
        ret.str = self->repr.s.data;
    }

    ret.status = SOS_OK;
    return ret;
}

SosStatus sos_init_from_range(Sos* self, const char* begin, size_t count)
{
    const SosStatusAndBuf ret = sos_init_for_overwrite(self, count);
    if (ret.status != SOS_OK) {
        return ret.status;
    }

    memcpy(ret.str, begin, count);
    // null byte is written by sos_init_from_range
    return SOS_OK;
}

SosStatus sos_init_from_cstr(Sos* self, const char* str)
{
    const size_t count = strlen(str);
    if (count <= SOS_SBO_BUFSIZE - 1) {
        memcpy(self->repr.s.data, str, count + 1);
        set_short_len(self, count);
        return SOS_OK;
    } else {
        char* const data = malloc((count | 1u) + 1);
        if (!data) {
            return SOS_ERROR_ALLOC;
        }
        memcpy(data, str, count + 1);
        self->repr.l.data = data;
        self->repr.l.len = count;
        self->repr.l.cap = count | 1u;
        return SOS_OK;
    }
}

SosStatus sos_init_adopt_cstr(Sos* self, char* str)
{
    // We might also move short str to inline buffer...

    const size_t len = strlen(str);
    self->repr.l.len = len;
    // Enforce capacity
    char* const data = realloc(str, (len | 1u) + 1);
    if (!data) {
        return SOS_ERROR_ALLOC;
    }
    self->repr.l.cap = len | 1u;
    self->repr.l.data = data;
    return SOS_OK;
}

SosStatus sos_init_format(Sos* self, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const size_t len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len + 1 > SOS_SBO_BUFSIZE) {
        char* const data = malloc((len | 1u) + 1);
        if (!data) {
            return SOS_ERROR_ALLOC;
        }
        va_start(args, fmt);
        vsnprintf(data, len + 1, fmt, args);
        va_end(args);
        self->repr.l.data = data;
        self->repr.l.len = len;
        self->repr.l.cap = len | 1u;
    } else {
        va_start(args, fmt);
        vsnprintf(self->repr.s.data, len + 1, fmt, args);
        va_end(args);
        set_short_len(self, len);
    }

    return SOS_OK;
}

void sos_finish(Sos* self)
{
    if (is_long(self)) {
        free(self->repr.l.data);
    }
    // Provide a safeguard, although self should not be used after sos_finish, unless re-initialized
    self->repr.s.len = 0;
    self->repr.s.data[0] = 0;
}

void sos_clear(Sos* self)
{
    if (is_long(self)) {
        // Shrinking a long-mode Sos never reverts it to short mode
        self->repr.l.data[0] = 0;
        self->repr.l.len = 0;
    } else {
        self->repr.s.data[0] = 0;
        self->repr.s.len = 0;
    }
}

/**
 * @pre `cap` must be odd and >= current size; `self` must be in short mode
 */
static SosStatus
sos_short_to_long(Sos* self, size_t cap)
{
    assert(!is_long(self) && cap % 2 == 1 && cap >= short_len(self));

    char* const data_new = malloc(cap + 1);
    if (!data_new) {
        return SOS_ERROR_ALLOC;
    }
    const size_t len = short_len(self);
    memcpy(data_new, self->repr.s.data, len + 1);

    self->repr.l.data = data_new;
    self->repr.l.len = len;
    self->repr.l.cap = cap;
    return SOS_OK;
}

static SosStatus
sos_reserve_long(Sos* self, size_t cap)
{
    if (cap > self->repr.l.cap) {
        cap |= 1u;
        char* const data_new = realloc(self->repr.l.data, cap + 1);
        if (!data_new) {
            return SOS_ERROR_ALLOC;
        }
        self->repr.l.data = data_new;
        self->repr.l.cap = cap;
    }

    return SOS_OK;
}

SosStatus sos_reserve(Sos* self, size_t cap)
{
    if (is_long(self)) {
        return sos_reserve_long(self, cap);
    } else if (cap + 1 > SOS_SBO_BUFSIZE) {
        cap |= 1u;
        return sos_short_to_long(self, cap);
    }

    return SOS_OK;
}

SosStatus sos_resize(Sos* self, size_t len, char ch)
{
    if (is_long(self)) {
        if (len > self->repr.l.len) { // This condition can be skipped.
            const SosStatus ret = sos_reserve_long(self, len);
            if (ret != SOS_OK) {
                return ret;
            }
            for (size_t i = self->repr.l.len; i < len; ++i) {
                self->repr.l.data[i] = ch;
            }
        }
        self->repr.l.data[len] = 0;
        self->repr.l.len = len;
    } else {
        const size_t current_len = short_len(self);
        if (len <= current_len) { // This condition can be skipped.
            self->repr.s.data[len] = 0;
            set_short_len(self, len);
        } else if (len + 1 <= SOS_SBO_BUFSIZE) {
            for (size_t i = current_len; i < len; ++i) {
                self->repr.s.data[i] = ch;
            }
            self->repr.s.data[len] = 0;
            set_short_len(self, len);
        } else {
            const SosStatus ret = sos_short_to_long(self, len | 1u);
            if (ret != SOS_OK) {
                return ret;
            }
            for (size_t i = current_len; i < len; ++i) {
                self->repr.l.data[i] = ch;
            }
            self->repr.l.data[len] = 0;
            self->repr.l.len = len;
        }
    }

    return SOS_OK;
}

void sos_shrink_to_fit(Sos* self)
{
    if (!is_long(self)) {
        return;
    }
    const size_t min_cap = self->repr.l.len | 1u;
    if (self->repr.l.cap > min_cap) {
        char* const data_new = realloc(self->repr.l.data, min_cap);
        if (!data_new) {
            return;
        }
        self->repr.l.data = data_new;
        self->repr.l.cap = min_cap;
    }
}

SosStatus sos_push(Sos* self, char c)
{
    if (is_long(self)) {
        if (self->repr.l.len == SOS_MAX_LEN) {
            return SOS_ERROR_MAX_CAP;
        }
        if (self->repr.l.cap == self->repr.l.len) {
            size_t cap_new;
            if (self->repr.l.cap > SOS_MAX_LEN_FOR_EXPAND) {
                cap_new = SOS_MAX_LEN;
            } else {
                cap_new = self->repr.l.cap * 2 + 1;
                if (cap_new < 31) {
                    cap_new = 31;
                }
            }
            char* const data_new = realloc(self->repr.l.data, cap_new + 1);
            if (!data_new) {
                return SOS_ERROR_ALLOC;
            }
            self->repr.l.data = data_new;
            self->repr.l.cap = cap_new;
        }
        self->repr.l.data[self->repr.l.len] = c;
        self->repr.l.data[self->repr.l.len + 1] = 0;
        self->repr.l.len += 1;
    } else {
        const unsigned char len = short_len(self);
        if (len == SOS_SBO_BUFSIZE - 1) {
            const SosStatus ret = sos_short_to_long(self, 31); //Should ensure SOS_SBO_BUFSIZE < 32(or else, and UCHAR_MAX) at compile time
            if (ret != SOS_OK) {
                return ret;
            }

            self->repr.l.data[self->repr.l.len] = c;
            self->repr.l.data[self->repr.l.len + 1] = 0;
            self->repr.l.len += 1;
        } else {
            self->repr.s.data[len] = c;
            self->repr.s.data[len + 1] = 0;
            self->repr.s.len += 2; // 0b10
        }
    }

    return SOS_OK;
}

char sos_pop(Sos* self)
{
    if (is_long(self)) {
        assert(self->repr.l.len > 0);

        const char ret = self->repr.l.data[self->repr.l.len - 1];
        self->repr.l.data[self->repr.l.len - 1] = 0;
        self->repr.l.len -= 1;
        return ret;
    }
    assert(short_len(self) > 0);

    const unsigned char len = short_len(self) - 1;
    const char ret = self->repr.s.data[len];
    self->repr.s.data[len] = 0;
    self->repr.s.len -= 2;
    return ret;
}

SosStatus sos_append_range(Sos* restrict self, const char* restrict begin, size_t count)
{
    if (!is_long(self)) {
        const size_t len = short_len(self);
        if (len + count <= SOS_SBO_BUFSIZE - 1) { //Check for max len
            memcpy(self->repr.s.data + len, begin, count);
            self->repr.s.data[len + count] = 0;
            self->repr.s.len += (unsigned char)count << 1;
            return SOS_OK;
        } else {
            const SosStatus ret = sos_short_to_long(self, (SOS_SBO_BUFSIZE + count) | 1u);
            if (ret != SOS_OK) {
                return ret;
            }
            memcpy(self->repr.l.data + len, begin, count);
            self->repr.l.data[len + count] = 0;
            self->repr.l.len += count;
            return SOS_OK;
        }
    }
    // long mode
    const SosStatus ret = sos_reserve_long(self, self->repr.l.len + count); //Check for max len
    if (ret != SOS_OK) {
        return ret;
    }

    memcpy(self->repr.l.data + self->repr.l.len, begin, count);
    self->repr.l.data[self->repr.l.len + count] = 0;
    self->repr.l.len += count;
    return SOS_OK;
}

SosStatus sos_append(Sos* restrict self, const Sos* restrict rhs)
{
    if (is_long(rhs)) {
        return sos_append_range(self, rhs->repr.l.data, rhs->repr.l.len);
    }
    return sos_append_range(self, rhs->repr.s.data, short_len(rhs));
}

SosStatus sos_append_cstr(Sos* restrict self, const char* restrict str)
{
    const size_t len = strlen(str);
    return sos_append_range(self, str, len);
}

SosStatus sos_init_by_copy(Sos* restrict self, const Sos* restrict rhs)
{
    memcpy(self, rhs, sizeof(Sos));
    if (is_long(rhs)) {
        char* const data = malloc(rhs->repr.l.cap + 1);
        if (!data) {
            return SOS_ERROR_ALLOC;
        }
        memcpy(data, rhs->repr.l.data, rhs->repr.l.len + 1);
        self->repr.l.data = data;
    }

    return SOS_OK;
}

void sos_init_by_move(Sos* restrict self, Sos* restrict rhs)
{
    sos_swap(self, rhs);
}

void sos_swap(Sos* restrict s1, Sos* restrict s2)
{
    Sos t;
    memcpy(&t, s1, sizeof(Sos));
    memcpy(s1, s2, sizeof(Sos));
    memcpy(s2, &t, sizeof(Sos));
}

static int
cmp_cstr(const char* lhs, const char* rhs)
{
    for (;; ++lhs, ++rhs) {
        if (*lhs > *rhs) {
            return 1;
        }
        if (*lhs < *rhs) {
            return -1;
        }
        if (*lhs == 0) {
            return 0;
        }
    }
}

int sos_cmp(const Sos* lhs, const Sos* rhs)
{
    return strcmp(sos_cstr(lhs), sos_cstr(rhs));
}

int sos_cmp_cstr(const Sos* lhs, const char* str)
{
    return strcmp(sos_cstr(lhs), str);
}


#if 0

int sos_ieq(const Sos* restrict lhs, const Sos* restrict rhs)
{
    const char* s1 = sos_cstr(lhs);
    const char* s2 = sos_cstr(rhs);

    for (;; ++s1, ++s2) {
        if (tolower(*s1) != tolower(*s2))
            return 0;
        if (*s1 == 0)
            return 1;
    }
}

#endif
