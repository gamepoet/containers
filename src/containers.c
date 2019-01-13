#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "containers.h"

static containers_lib_config_t s_config;

void* default_alloc(size_t size_bytes, void* allocator, const char* file, int line, const char* func) {
  return malloc(size_bytes);
}

void default_free(void* ptr, void* allocator, const char* file, int line, const char* func) {
  free(ptr);
}

void default_assert_failed(const char* expression, const char* message, const char* file, int line, const char* func) {
  fprintf(stderr, "ASSERTION FAILED\nexpression: %s\nmessage: %s\nfile: %s\nline: %d\nfunction: %s\n", expression, message, file, line, func);
}

void containers_lib_config_init(containers_lib_config_t* config) {
  if (config == NULL) {
    return;
  }

  config->alloc = &default_alloc;
  config->free = &default_free;
  config->assert_failed = &default_assert_failed;
}

void containers_lib_init(const containers_lib_config_t* config) {
  if (config == NULL) {
    containers_lib_config_init(&s_config);
  }
  else {
    s_config = *config;
  }
}

void containers_lib_shutdown() {
  memset(&s_config, 0, sizeof(s_config));
}

void containers__array_free_impl(void* arr, void* allocator, const char* file, int line, const char* func) {
  void* ptr = array__header(arr);
  s_config.free(ptr, allocator, file, line, func);
}

void* containers__array_grow_impl(void* arr, uint32_t inc, uint32_t item_size, void* allocator, const char* file, int line, const char* func) {
  // compute the new capacity
  const uint32_t count_old = array_count(arr);
  const uint32_t capacity_old = (arr == NULL) ? 0 : array__raw_capacity(arr);
  const uint32_t capacity_required = count_old + inc;
  const uint32_t capacity_doubled = 2 * capacity_old;
  const uint32_t capacity_new = capacity_required > capacity_doubled ? capacity_required : capacity_doubled;

  // realloc
  array_header_t* ptr_new = (array_header_t*)s_config.alloc((capacity_new * item_size) + sizeof(array_header_t), allocator, file, line, func);
  array_header_t* ptr_old = (arr == NULL) ? NULL : array__header(arr);
  if (ptr_old != NULL) {
    memmove(ptr_new, ptr_old, (count_old * item_size) + sizeof(array_header_t));
    s_config.free(ptr_old, allocator, file, line, func);
  }

  // fix the header
  ptr_new->capacity = capacity_new;
  if (ptr_old == NULL) {
    ptr_new->count = 0;
  }

  return ptr_new + 1;
}

void containers__array_memcpy(void* dest, const void* src, uint32_t size_bytes) {
  memcpy(dest, src, size_bytes);
}

void containers__array_check_min_count(void* arr, uint32_t min_count, const char* file, int line, const char* func) {
  if (array_count(arr) < min_count) {
    char message[64];
    snprintf(message, 64, "array must contain at least %u element%s", min_count, min_count == 1 ? "" : "s");
    s_config.assert_failed("array_count(arr) < count_min", message, file, line, func);
  }
}
