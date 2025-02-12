#ifndef SOS_H
#define SOS_H

/* SOS: Small-buffer Optimized String */

#include <stddef.h>
#include "sos_endian.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The least significant bit of cap indicates long/short mode */
#ifdef SOS_BE
typedef struct {
    size_t len;
    char* data;
    size_t cap;
} _sos_long;
#else
typedef struct {
    size_t cap;
    size_t len;
    char* data; // Invariant: cannot be null
} _sos_long;
#endif

#define SOS_SBO_BUFSIZE (sizeof(_sos_long) - 1)

#ifdef SOS_BE
typedef struct {
    char data[SOS_SBO_BUFSIZE];
    unsigned char len;
} _sos_short;
#else
typedef struct {
    unsigned char len; /* To provide constant-time length access */
    char data[SOS_SBO_BUFSIZE];
} _sos_short;
#endif

typedef struct {
    union {
        _sos_long  l;
        _sos_short s;
    } repr; /* anonymous union is C11 */
} Sos;

/* Return codes */
enum {
    SOS_OK = 0,
    SOS_ERROR_ALLOC,
    SOS_ERROR_MAX_CAP
};

/* Functions */

size_t sos_size(const Sos* self);
size_t sos_cap(const Sos* self);
const char* sos_str(const Sos* self);

void sos_init(Sos* self);
int sos_init_with_cap(Sos* self, size_t cap);
int sos_init_from_str(Sos* self, const char* str);
void sos_finish(Sos* self); /* Release resources. Does nothing in short mode. */

void sos_clear(Sos* self); /* Empty the string */

int sos_reserve(Sos* self, size_t cap);

int sos_push(Sos* self, char c);
char sos_pop(Sos* self); /* must be non-empty */
int sos_append(Sos* restrict self, const Sos* restrict rhs);
int sos_append_str(Sos* restrict self, const char* restrict str);
int sos_append_range(Sos* restrict self, const char* restrict begin, size_t count);

int sos_copy(Sos* restrict self, const Sos* restrict rhs);
void sos_move(Sos* restrict self, Sos* restrict rhs);
void sos_swap(Sos* restrict s1, Sos* restrict s2);

int sos_eq(const Sos* restrict lhs, const Sos* restrict rhs);
int sos_ieq(const Sos* restrict lhs, const Sos* restrict rhs); /* case insensitive */

#ifdef __cplusplus
}
#endif

#endif /* SOS_H */
