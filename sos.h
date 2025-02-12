#ifndef SOS_H
#define SOS_H

// SOS: Small-buffer Optimized String

#include <stddef.h>
#include "sos_endian.h"

#ifdef __cplusplus
extern "C" {
#endif

// The least significant bit of cap indicates long/short mode
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
    char* data; // Invariant: never null
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
    unsigned char len;
    char data[SOS_SBO_BUFSIZE];
} _sos_short;
#endif

typedef struct {
    union {
        _sos_long  l;
        _sos_short s;
    } repr; // anonymous union is C11
} Sos;

// Return codes
typedef enum {
    SOS_OK = 0,
    SOS_ERROR_ALLOC,
    SOS_ERROR_MAX_CAP
} SOS_STATUS;

// Observers

/**
 * Get current length of string (not including the null characater).
 *
 * @pre `self` is initialized
 */
size_t sos_size(const Sos* self);

/**
 * Get capacity of the managed buffer (not including the null characater).
 *
 * @pre `self` is initialized
 */
size_t sos_cap(const Sos* self);

/**
 * Get the null-terminated C string.
 *
 * @pre `self` is initialized
 */
const char* sos_str(const Sos* self);

// Functions for initialization and life-time management.
// Initialization functions will not check if `self` holds any resources and release them.

/**
 * Initialize an empty string.
 *
 * @pre `self` isn't initialized, or is in short mode.
 * @post `self` is in short mode, with zero length.
 */
void sos_init(Sos* self);

/**
 * Initialize with a minimum capacity.
 *
 * @pre `self` isn't initialized, or is in short mode.
 * @post `self` is initialized to either short mode or long mode. Its capacity is at-least `cap`.
 */
SOS_STATUS sos_init_with_cap(Sos* self, size_t cap);

/**
 * Initialize with a given C string.
 *
 * @pre `self` isn't initialized, or is in short mode.
 * @post `self` is initialized to either short mode or long mode, with the same content as `str`.
 */
SOS_STATUS sos_init_from_str(Sos* self, const char* str);

/**
 * Release allocated memory, if any.
 *
 * @pre `self` is initialized
 * @post `self` is uninitialized
 */
void sos_finish(Sos* self);

/**
 * Swap two strings.
 */
void sos_swap(Sos* restrict s1, Sos* restrict s2);

/**
 * Initialize a string by copying from another.
 * 
 * @pre `self` isn't initialized, or is in short mode.
 *      `rhs` is initialized.
 * @post `self` is initialized by copying rhs.
 */
SOS_STATUS sos_copy(Sos* restrict self, const Sos* restrict rhs);

/**
 * Same as sos_swap
 */
void sos_move(Sos* restrict self, Sos* restrict rhs);

// Modifiers

/**
 * Set length of string to zero.
 *
 * @pre `self` is initialized
 * @post `self` has zero length. Its capacity is not modified.
 */
void sos_clear(Sos* self);

/**
 * Reserve capacity for string.
 *
 * @pre `self` is initialized
 * @post `self` has capacity of at-least `cap`
 */
SOS_STATUS sos_reserve(Sos* self, size_t cap);

/**
 * Push-back a character.
 *
 * @pre `self` is initialized
 */
SOS_STATUS sos_push(Sos* self, char c);

/**
 * Pop-back a character.
 *
 * @pre `self` is initialized and not empty.
 */
char sos_pop(Sos* self);

/**
 * Append another `sos` string.
 *
 * @pre `self` and `rhs` is initialized
 */
SOS_STATUS sos_append(Sos* restrict self, const Sos* restrict rhs);

/**
 * Append a C string.
 *
 * @pre `self` is initialized
 */
SOS_STATUS sos_append_str(Sos* restrict self, const char* restrict str);

/**
 * Append a contiguous range of chars.
 *
 * @pre `self` is initialized
 */
SOS_STATUS sos_append_range(Sos* restrict self, const char* restrict begin, size_t count);

// Comparison

/**
 * Test if two strings are equal.
 */
int sos_eq(const Sos* restrict lhs, const Sos* restrict rhs);

/**
 * Test if two strings are equal.
 */
int sos_eq_str(const Sos* restrict lhs, const char* restrict str);

/**
 * Test if two strings are equal, case insensitive.
 */
int sos_ieq(const Sos* restrict lhs, const Sos* restrict rhs);

#ifdef __cplusplus
}
#endif

#endif // SOS_H
