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
    char* data; // Invariant: never null in long mode
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

// Methods
// Initialization functions require the argument to be uninitialized.
// All other operations expect initialized Sos, unless otherwise noted
// Violating this precondition leads to undefined behavior.

// An Sos string being in short mode or long mode is treated as an implementation detail,
// deliberately hidden from the public API.

// Observers

/**
 * Get current length of string (not including the null character).
 */
size_t sos_len(const Sos* self);

/**
 * Get capacity of the managed buffer (not including the null character).
 */
size_t sos_cap(const Sos* self);

// Deep constness is used, since Sos has exclusive ownership over the char array.

/**
 * Get the null-terminated C string (immutable).
 */
const char* sos_cstr(const Sos* self);

/**
 * Get the null-terminated C string (mutable).
 */
char* sos_cstr_mut(Sos* self);

// Functions for initialization and life-time management.
// Initialization functions will not check if `self` holds any resources and release them.

/**
 * Initialize an empty string.
 *
 * @pre `self` is not initialized.
 * @post `self` is initialized, with zero length.
 */
void sos_init(Sos* self);

/**
 * Initialize with a minimum capacity.
 *
 * @pre `self` is not initialized.
 * @post `self` is initialized. Its capacity is at-least `cap`.
 */
SOS_STATUS sos_init_with_cap(Sos* self, size_t cap);

/**
 * Initialize by copying a given C string.
 *
 * @pre `self` is not initialized.
 * @post `self` is initialized, with the same content as `str`.
 */
SOS_STATUS sos_init_from_cstr(Sos* self, const char* str);

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
 * Initialize by copying from another.
 * 
 * @pre `self` is not initialized.
 *      `rhs` is initialized.
 * @post `self` is initialized by copying rhs.
 */
SOS_STATUS sos_init_by_copy(Sos* restrict self, const Sos* restrict rhs);

/**
 * Initialize by moving from another.
 * 
 * @pre `self` is not initialized.
 *      `rhs` is initialized.
 * @post `self` is initialized by moving from `rhs`.
 *       `rhs` is uninitialized.
 * @note Internally this is same as sos_swap
 */
void sos_init_by_move(Sos* restrict self, Sos* restrict rhs);

// Modifiers

/**
 * Set length of string to zero.
 *
 * @post `self` has zero length. Its capacity is not modified.
 */
void sos_clear(Sos* self);

/**
 * Reserve capacity for string.
 *
 * @post `self` has capacity of at-least `cap`
 */
SOS_STATUS sos_reserve(Sos* self, size_t cap);

/**
 * Shrink unused capacity, if possible.
 */
void sos_shrink_to_fit(Sos* self);

/**
 * Push-back a character.
 */
SOS_STATUS sos_push(Sos* self, char c);

/**
 * Pop-back a character.
 *
 * @pre `self` is not empty.
 * @return The character that was popped out.
 */
char sos_pop(Sos* self);

/**
 * Append another `sos` string.
 */
SOS_STATUS sos_append(Sos* restrict self, const Sos* restrict rhs);

/**
 * Append a C string.
 */
SOS_STATUS sos_append_cstr(Sos* restrict self, const char* restrict str);

/**
 * Append a contiguous range of chars.
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
int sos_eq_cstr(const Sos* restrict lhs, const char* restrict str);

/**
 * Test if two strings are equal, case insensitive.
 */
int sos_ieq(const Sos* restrict lhs, const Sos* restrict rhs);

#ifdef __cplusplus
}
#endif

#endif // SOS_H
