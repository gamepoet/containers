#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "containers.h"

static const uint32_t HASH_INITIAL_CAPACITY = 128;
static const uint32_t HASH_LOAD_FACTOR_PERCENT = 90;

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

static void hash_insert_impl(hash_t* hash, uint32_t key, uint32_t value) {
  ++hash->count;

  uint32_t* keys = hash->keys;
  uint32_t* values = hash->values;
  const uint32_t capacity = hash->capacity;
  const uint32_t mask = (capacity - 1);

  const uint32_t index_desired = key & mask;
  uint32_t index = index_desired;
  uint32_t distance = 0;
  for (;;) {
    const uint32_t key_cur = keys[index];
    // if the current index is empty, use it
    if (key_cur == 0) {
      keys[index] = key;
      values[index] = value;
      break;
    }

    // if the existing element has probled less than us, swap places and look for a place for the existing element
    const uint32_t distance_existing = (index + capacity - (key_cur & mask)) & mask;
    if (distance_existing < distance) {
      uint32_t tmp_key = keys[index];
      uint32_t tmp_value = values[index];
      keys[index] = key;
      values[index] = value;
      key = tmp_key;
      value = tmp_value;
      distance = distance_existing;
    }

    // linear probing
    index = (index + 1) & mask;
    ++distance;
  }
}

static void hash_grow(hash_t* hash, void* allocator) {
  const uint32_t capacity_desired = hash->capacity * 2;
  const uint32_t capacity_new = capacity_desired < HASH_INITIAL_CAPACITY ? HASH_INITIAL_CAPACITY : capacity_desired;

  // alloc new key and value arrays
  uint32_t* keys_new = (uint32_t*)s_config.alloc(capacity_new * sizeof(uint32_t), allocator, __FILE__, __LINE__, __func__);
  uint32_t* values_new = (uint32_t*)s_config.alloc(capacity_new * sizeof(uint32_t), allocator, __FILE__, __LINE__, __func__);

  // mark all buckets as empty
  memset(keys_new, 0, capacity_new * sizeof(uint32_t));

  // swap out the hash data the new and old arrays
  const uint32_t capacity_old = hash->capacity;
  uint32_t* keys_old = hash->keys;
  uint32_t* values_old = hash->values;
  hash->keys = keys_new;
  hash->values = values_new;
  hash->count = 0;
  hash->capacity = capacity_new;

  // reinsert the old elements
  for (uint32_t index = 0; index < capacity_old; ++index) {
    const uint32_t key_old = keys_old[index];
    const uint32_t value_old = values_old[index];
    if (key_old != 0) {
      hash_insert_impl(hash, key_old, value_old);
    }
  }

  // cleanup
  s_config.free(keys_old, allocator, __FILE__, __LINE__, __func__);
  s_config.free(values_old, allocator, __FILE__, __LINE__, __func__);
}

uint32_t hash_count(const hash_t* hash) {
  return hash->count;
}

void hash_free(hash_t* hash, void* allocator) {
  if (hash->capacity > 0) {
    s_config.free(hash->keys, allocator, __FILE__, __LINE__, __func__);
    s_config.free(hash->values, allocator, __FILE__, __LINE__, __func__);
  }
  hash->keys = NULL;
  hash->values = NULL;
  hash->count = 0;
  hash->capacity = 0;
}

void hash_insert(hash_t* hash, uint32_t key, uint32_t value, void* allocator) {
  const uint32_t resize_threshold = (hash->capacity * HASH_LOAD_FACTOR_PERCENT) / 100;
  if (hash->count >= resize_threshold) {
    hash_grow(hash, allocator);
  }
  hash_insert_impl(hash, key, value);
}

uint32_t hash_lookup(const hash_t* hash, uint32_t key, uint32_t default_value) {
  const uint32_t* keys = hash->keys;
  const uint32_t* values = hash->values;
  const uint32_t capacity = hash->capacity;
  const uint32_t mask = capacity - 1;

  // nothing to find in an empty table
  if (capacity == 0) {
    return default_value;
  }

  uint32_t index = key & mask;
  uint32_t distance = 0;
  for (;;) {
    const uint32_t key_cur = keys[index];

    // found a match
    if (key_cur == key) {
      return values[index];
    }

    // found an empty slot; not found
    if (key_cur == 0) {
      return default_value;
    }

    // we've probed farther than the current slot's distance; implies not found
    const uint32_t distance_existing = (index + capacity - (key_cur & mask)) & mask;
    if (distance > distance_existing) {
      return default_value;
    }

    // probe the next slot
    index = (index + 1) & mask;
    ++distance;
  }
}

bool hash_contains(const hash_t* hash, uint32_t key) {
  const uint32_t* keys = hash->keys;
  const uint32_t capacity = hash->capacity;
  const uint32_t mask = capacity - 1;

  // nothing to find in an empty table
  if (capacity == 0) {
    return false;
  }

  uint32_t index = key & mask;
  uint32_t distance = 0;
  for (;;) {
    const uint32_t key_cur = keys[index];

    // found a match
    if (key_cur == key) {
      return true;
    }

    // found an empty slot; not found
    if (key_cur == 0) {
      return false;
    }

    // we've probed farther than the current slot's distance; implies not found
    const uint32_t distance_existing = (index + capacity - (key_cur & mask)) & mask;
    if (distance > distance_existing) {
      return false;
    }

    // probe the next slot
    index = (index + 1) & mask;
    ++distance;
  }

  // if we got here, there was an error
  return false;
}

void hash_remove(hash_t* hash, uint32_t key) {
  uint32_t* keys = hash->keys;
  uint32_t* values = hash->values;
  const uint32_t capacity = hash->capacity;
  const uint32_t mask = capacity - 1;

  // nothing to remove in an empty table
  if (capacity == 0) {
    return;
  }

  uint32_t index = key & mask;
  uint32_t distance = 0;
  for (;;) {
    const uint32_t key_cur = keys[index];

    // found a match
    if (key_cur == key) {
      break;
    }

    // found an empty slot; not found
    if (key_cur == 0) {
      return;
    }

    // we've probed farther than the current slot's distance; implies not found
    const uint32_t distance_existing = (index + capacity - (key_cur & mask)) & mask;
    if (distance > distance_existing) {
      return;
    }

    // probe the next slot
    index = (index + 1) & mask;
    ++distance;
  }

  // now backshift the remaining elements whole distance is greater than zero
  for (uint32_t offset = 1; offset < capacity; ++offset) {
    const uint32_t index_src = (index + offset) & mask;
    const uint32_t key_src = keys[index_src];

    // src slot is empty; nothing left to move
    if (key_src == 0) {
      break;
    }

    // src slot is in a perfect position; nothing left to move
    const uint32_t distance_existing = (index_src + capacity - (key_src & mask)) & mask;
    if (distance_existing == 0) {
      break;
    }

    // move the slot up
    const uint32_t index_dst = (index + offset - 1) & mask;
    keys[index_dst] = keys[index_src];
    values[index_dst] = values[index_src];
  }

  --hash->count;
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
