#ifndef SOS_H
#define SOS_H

// SOS: Small-buffer Optimized String

#include <stddef.h>
#include <stdbool.h>
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
} SosStatus;

// A struct to hold status code along with pointer to string buffer.
// Used as a return type.
typedef struct {
    SosStatus status;
    char* str;
} SosStatusAndBuf;

// Struct representing a mutable string view
typedef struct {
    char* data;
    size_t len;
} SosViewMut;

// Struct representing an immutable string view
typedef struct {
    const char* data;
    size_t len;
} SosView;


// Methods
// Initialization functions require the argument to be uninitialized.
// All other operations expect initialized Sos, unless otherwise noted.
// Violating this precondition leads to undefined behavior.

// If any of the initialization functions fails, the argument is not initialized.

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

/**
 * Get an immutable string view.
 */
SosView sos_view(const Sos* self);

/**
 * Get a mutable string view.
 */
SosViewMut sos_view_mut(Sos* self);

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
 * @post On success, `self` is initialized with zero length. Its capacity is at-least `cap`.
 */
SosStatus sos_init_with_cap(Sos* self, size_t cap);

/**
 * Initialize with given length, leaving content of the string uninitialized.
 *
 * @pre `self` is not initialized.
 * @post On success, `self` is initialized and has length `len`.
 *       The content of the string is not initialized, but the terminating null byte is written.
 * @return Status code, plus the managed string buffer on success.
 */
SosStatusAndBuf sos_init_for_overwrite(Sos* self, size_t len);

/**
 * Initialize by copying the given char range.
 *
 * @pre `self` is not initialized.
 * @post On success, `self` is initialized by copying the range.
 */
SosStatus sos_init_from_range(Sos* self, const char* begin, size_t count);

/**
 * Initialize by copying a given C string.
 *
 * @pre `self` is not initialized.
 * @post On success, `self` is initialized, with the same content as `str`.
 */
SosStatus sos_init_from_cstr(Sos* self, const char* str);

/**
 * Initialize by adopting a C string.
 *
 * @param[in] str The C string to adopt.
 *                It must have been created by malloc(), calloc(), realloc() etc.
 *                Ownership is transferred into `self`, so `self` is responsible for freeing it.
 * @pre `self` is not initialized.
 */
SosStatus sos_init_adopt_cstr(Sos* self, char* str);
// We might want to add a function to adopt C string with known size/capacity.

/**
 * Initialize using a format string and arguments
 *
 * @param[in] fmt `printf`-style format string
 * @pre `self` is not initialized.
 */
SosStatus sos_init_format(Sos* self, const char* fmt, ...);

/**
 * Destroy a string, making `self` uninitialized.
 * This will release allocated memory, if any.
 *
 * @pre `self` is initialized.
 * @post `self` is uninitialized.
 */
void sos_finish(Sos* self);

/**
 * Transfer ownership of the string out of `self`.
 * Afterwards, the user is responsible for freeing it.
 *
 * @return The C string that was managed by `self`, along with its length. It should be freed by calling free() on it.
 *         The returned C string can be NULL if allocation fails.
 * @pre `self` is initialized.
 * @post `self` is uninitialized.
 */
SosViewMut sos_release(Sos* self);

/**
 * Swap two strings.
 */
void sos_swap(Sos* restrict s1, Sos* restrict s2);

/**
 * Initialize by copying from another.
 *
 * @pre `self` is not initialized.
 *      `rhs` is initialized.
 * @post On success, `self` is initialized by copying rhs.
 */
SosStatus sos_init_by_copy(Sos* restrict self, const Sos* restrict rhs);

/**
 * Initialize by moving from another.
 *
 * @pre `self` is not initialized.
 *      `rhs` is initialized.
 * @post `self` is initialized by moving from `rhs`.
 *       `rhs` is uninitialized.
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
 * Set length of string.
 * If `len` is greater than current length, append `ch`.
 *
 * @post On success, length of string is `len`.
 */
SosStatus sos_resize(Sos* self, size_t len, char ch);

/**
 * Reserve capacity for string.
 *
 * @post On success, `self` has capacity of at-least `cap`
 */
SosStatus sos_reserve(Sos* self, size_t cap);

/**
 * Shrink unused capacity, if possible.
 */
void sos_shrink_to_fit(Sos* self);

/**
 * Push-back a character.
 */
SosStatus sos_push(Sos* self, char c);

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
SosStatus sos_append(Sos* restrict self, const Sos* restrict rhs);

/**
 * Append a C string.
 */
SosStatus sos_append_cstr(Sos* restrict self, const char* restrict str);

/**
 * Append a contiguous range of chars.
 */
SosStatus sos_append_range(Sos* restrict self, const char* restrict begin, size_t count);

/**
 * Enlarge the string, but does not initialize the expanded content.
 *
 * @post On success, length of string is increased by `count`.
 *       The expanded chars are uninitialized, but the terminating null byte is written.
 * @return Status code, along with a pointer to the first character of the expanded range.
 *         If `count` is zero, the returned pointer points to the terminating null byte.
 */
SosStatusAndBuf sos_expand_for_overwrite(Sos* self, size_t count);

// Comparison

/**
 * Compare two strings, as if comparing with strcmp.
 *
 * @return See strcmp
 */
int sos_cmp(const Sos* lhs, const Sos* rhs);

/**
 * Compare string with C string.
 */
int sos_cmp_cstr(const Sos* lhs, const char* str);

/**
 * Test if two strings are equal.
 *
 * @note This could be more efficient than sos_cmp for equality test.
 */
bool sos_eq(const Sos* lhs, const Sos* rhs);

#ifdef __cplusplus
}
#endif

#endif // SOS_H
