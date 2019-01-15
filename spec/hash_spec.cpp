#include "utils.h"

TEST_CASE("hash") {
  init_t init(NULL);

  SECTION("it can insert and lookup correctly") {
    hash_t hash = {};
    CHECK(hash_count(&hash) == 0);
    hash_insert(&hash, 25, 1, NULL);
    CHECK(hash_count(&hash) == 1);
    CHECK(hash_lookup(&hash, 25, 0) == 1);
    hash_free(&hash, NULL);
  }

  SECTION("it can remove correctly") {
    hash_t hash = {};
    hash_insert(&hash, 25, 1, NULL);
    hash_insert(&hash, 50, 2, NULL);
    CHECK(hash_count(&hash) == 2);
    hash_remove(&hash, 25);
    CHECK(hash_count(&hash) == 1);
    hash_remove(&hash, 50);
    CHECK(hash_count(&hash) == 0);
    hash_free(&hash, NULL);
  }

  SECTION("it reports containment") {
    hash_t hash = {};
    CHECK(!hash_contains(&hash, 25));
    hash_insert(&hash, 25, 1, NULL);
    CHECK(hash_contains(&hash, 25));
    hash_free(&hash, NULL);
  }

  SECTION("hash_remove gracefully handles an empty table") {
    hash_t hash = {};
    hash_remove(&hash, 1234);
    CHECK(hash_count(&hash) == 0);
  }

  SECTION("hash_reserve rounds up to the next pow 2") {
    hash_t hash = {};
    hash_reserve(&hash, 300, NULL);
    CHECK(hash_capacity(&hash) == 512);
    hash_free(&hash, NULL);
  }

  SECTION("items survive growth") {
    hash_t hash = {};
    for (uint32_t index = 1; index < 64; ++index) {
      hash_insert(&hash, index, index, NULL);
    }
    CHECK(hash_capacity(&hash) == 128);
    for (uint32_t index = 64; index < 128; ++index) {
      hash_insert(&hash, index, index, NULL);
    }
    CHECK(hash_capacity(&hash) == 256);
    for (uint32_t index = 1; index < 128; ++index) {
      CHECK(hash_lookup(&hash, index, 0) == index);
    }
    hash_free(&hash, NULL);
  }

  SECTION("hash_lookup gracefully handles an empty table") {
    hash_t hash = {};
    CHECK(hash_lookup(&hash, 1, 0) == 0);
  }

  SECTION("hash_lookup returns the default value if not found") {
    hash_t hash = {};
    hash_insert(&hash, 1, 1, NULL);
    CHECK(hash_lookup(&hash, 2, 0) == 0);
    hash_free(&hash, NULL);
  }

  SECTION("hash_contains handles an empty table") {
    hash_t hash = {};
    CHECK(!hash_contains(&hash, 1234));
  }

  SECTION("hash_contains handles return false if not found") {
    hash_t hash = {};
    hash_insert(&hash, 1, 1, NULL);
    CHECK(!hash_contains(&hash, 2));
    hash_free(&hash, NULL);
  }

  SECTION("hash_insert does robin hood hashing") {
    hash_t hash = {};
    hash_insert(&hash, 1, 1, NULL);
    hash_insert(&hash, 2, 2, NULL);
    hash_insert(&hash, 129, 129, NULL);
    CHECK(hash.keys[1] == 1);
    CHECK(hash.keys[2] == 129);
    CHECK(hash.keys[3] == 2);
    hash_free(&hash, NULL);
  }
}

TEST_CASE("hash with custom alloc") {
  containers_lib_config_t config;
  containers_lib_config_init(&config);
  config.alloc = [](size_t size, void* allocator, const char* file, int line, const char* func) {
    ++(*(uint32_t*)allocator);
    return malloc(size);
  };
  config.free = [](void* ptr, void* allocator, const char* file, int line, const char* func) {
    --(*(uint32_t*)allocator);
    free(ptr);
  };
  init_t init(&config);

  SECTION("the allocator is passed to the alloc and free funcs") {
    hash_t hash_a = {};
    hash_t hash_b = {};
    uint32_t allocator_a = 0;
    uint32_t allocator_b = 0;
    hash_insert(&hash_a, 1, 1, &allocator_a);
    CHECK(allocator_a == 2);
    hash_insert(&hash_b, 1, 1, &allocator_b);
    CHECK(allocator_b == 2);
    hash_reserve(&hash_a, 300, &allocator_a);
    CHECK(allocator_a == 2);
    hash_reserve(&hash_b, 300, &allocator_b);
    CHECK(allocator_b == 2);
    hash_free(&hash_a, &allocator_a);
    CHECK(allocator_a == 0);
    hash_free(&hash_b, &allocator_b);
    CHECK(allocator_b == 0);
  }
}
