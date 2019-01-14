#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
#define CONTAINERS_CHECK_ENABLED 1
#endif

//
// Array
//
// This array type is implemented as a "stretchy buffer" (https://github.com/nothings/stb/blob/master/stretchy_buffer.h).
// Basically this is a set of macros that manipulate what looks like a raw pointer but instead has the count and
// capacity stored in the bytes just before the start of the array.
//

typedef struct array_header_t {
  uint32_t capacity;
  uint32_t count;
} array_header_t;

// clang-format off

// INTERNAL
#define array__header(arr)                            ((array_header_t*)((char*)(arr) - sizeof(array_header_t)))
#define array__raw_count(arr)                         (array__header(arr)->count)
#define array__raw_capacity(arr)                      (array__header(arr)->capacity)
#define array__should_grow(arr, inc)                  ((arr) == 0 || array__raw_count(arr) + (inc) > array__raw_capacity(arr))
#define array__maybe_grow(arr, inc, allocator)        (array__should_grow(arr, inc) ? array__grow(arr, inc, allocator) : 0)
#define array__grow(arr, inc, allocator)              (*((void**)&(arr)) = containers__array_grow_impl(arr, inc, sizeof(*(arr)), allocator, __FILE__, __LINE__, __func__))
#ifdef CONTAINERS_CHECK_ENABLED
# define array__check_min_count(arr, count)           (containers__array_check_min_count(arr, count, __FILE__, __LINE__, __func__))
# define array__check_not_empty(arr)                  (containers__array_check_min_count(arr, 1, __FILE__, __LINE__, __func__))
#else
# define array__check_min_count(arr, count)           ((void*)0)
# define array__check_not_empty(arr)                  ((void*)0)
#endif

// Gets the number of elements currently stored in the array.
#define array_count(arr)                              ((arr) ? array__raw_count(arr) : 0)

// Gets the current capacity of the array (total count before re-allocation must occur).
#define array_capacity(arr)                           ((arr) ? array__raw_capacity(arr) : 0)

// Frees the array and effectively empties it.
#define array_free(arr, allocator)                    ((arr) ? (containers__array_free_impl(arr, allocator, __FILE__, __LINE__, __func__), *((void**)&(arr)) = 0, 0) : 0)

// Ensures there is enough capacity in the array to hold *cap* elements.
#define array_reserve(arr, cap, allocator)            ((cap) > array_capacity(arr) ? (array__grow(arr, (cap) - array_capacity(arr), allocator), (cap)) : 0)

// Ensures there is enough capacity in the array to grow by *inc* elements.
#define array_reserve_more(arr, inc, allocator)       (array__maybe_grow(arr, inc, allocator))

// Convenience function to get the first element of the array. NOTE: the array must not be empty.
#define array_first(arr)                              (array__check_not_empty(arr), (arr)[0])

// Convenience function to get the last element of the array. NOTE: the array must not be empty
#define array_last(arr)                               (array__check_not_empty(arr), (arr)[array__raw_count(arr) - 1])

// Pushes an element onto the end of the array, growing more capacity if required.
#define array_push(arr, val, allocator)               (array__maybe_grow(arr, 1, allocator), (arr)[array__raw_count(arr)++] = (val))

// Pops an element off the end of the array. NOTE: the array must not be empty.
#define array_pop(arr)                                (array__check_not_empty(arr), --array__raw_count(arr))

// Removes the first element from the array, moving the remaining elements up.
#define array_shift(arr)                              (array__check_not_empty(arr), containers__array_memcpy(arr, (arr) + 1, sizeof(*(arr)) * (array__raw_count(arr)--)))
#define array_pop_front(arr)                          (array_shift(arr))

// Adds an element to the front of the array, moving the remaining elements down and growing more capacity if required.
#define array_unshift(arr, val, allocator)            (array__maybe_grow(arr, 1, allocator), containers__array_memcpy((arr) + 1, arr, sizeof(*(arr)) * (array__raw_count(arr)++)), (arr)[0] = (val))
#define array_push_front(arr, val, allocator)         (array_unshift(arr, val, allocator))

// Adds N elements onto the end of the array, growing more capacity if required.
#define array_push_n(arr, items, n, allocator)        (array__maybe_grow(arr, n, allocator), containers__array_memcpy((arr) + array__raw_count(arr), items, sizeof(*(arr)) * (n)), array__raw_count(arr) += (n))

// Removes N elements from the end of the array. NOTE: the array count must at least that large
#define array_pop_n(arr, n)                           (array__check_min_count(arr, n), array__raw_count(arr) -= (n))

// Removes N elements from the front of the array, moving the remaining elements up.
#define array_shift_n(arr, n)                         (array__check_min_count(arr, n), containers__array_memcpy(arr, (arr) + (n), sizeof(*(arr)) * (array__raw_count(arr))), array__raw_count(arr) -= (n))
#define array_pop_front_n(arr, n)                     (array_shift_n(arr, n))

// Adds N elements to the front of the array, moving the remaining elements down and growing more capacity if required.
#define array_unshift_n(arr, items, n, allocator)     (array__maybe_grow(arr, n, allocator), containers__array_memcpy((arr) + (n), arr, sizeof(*(arr)) * (array__raw_count(arr))), containers__array_memcpy(arr, items, (n) * sizeof(*(arr))), array__raw_count(arr) += (n))
#define array_push_front_n(arr, items, n, allocator)  (array_unshift_n(arr, items, n, allocator))

// clang-format on

// INTERNAL
void containers__array_free_impl(void* arr, void* allocator, const char* file, int line, const char* func);
void* containers__array_grow_impl(void* arr, uint32_t increment, uint32_t item_size, void* allocator, const char* file, int line, const char* func);
void containers__array_memcpy(void* dest, const void* src, uint32_t size_bytes);
void containers__array_check_min_count(void* arr, uint32_t min_count, const char* file, int line, const char* func);

//
// Hash
//
// This container is a hashtable without storage. Basically the hashtable stores the key hashes and value indices. It is
// up to the user of the container to actually store the key and value data.
//

typedef struct hash_t {
  uint32_t* keys;
  uint32_t* values;
  uint32_t capacity;
  uint32_t count;
} hash_t;

uint32_t hash_count(const hash_t* hash);
void hash_free(hash_t* hash, void* allocator);
void hash_insert(hash_t* hash, uint32_t key, uint32_t value, void* allocator);
uint32_t hash_lookup(const hash_t* hash, uint32_t key, uint32_t default_value);
void hash_remove(hash_t* hash, uint32_t key);
bool hash_contains(const hash_t* hash, uint32_t key);

//
// Library initialization and configuration
//

typedef struct containers_lib_config_t {
  // The function used to allocate memory. The default implementation is malloc().
  void* (*alloc)(size_t size, void* allocator, const char* file, int line, const char* func);

  // The function used to free memory. The default implementation is free().
  void (*free)(void* ptr, void* allocator, const char* file, int line, const char* func);

  // The function used when an assertion fails.
  void (*assert_failed)(const char* expression, const char* message, const char* file, int line, const char* func);
} containers_lib_config_t;

// Initializes the given config struct to fill it in with the default values.
void containers_lib_config_init(containers_lib_config_t* config);

// Initializes this library.
void containers_lib_init(const containers_lib_config_t* config);

// Tears down this library.
void containers_lib_shutdown();

#ifdef __cplusplus
}
#endif
