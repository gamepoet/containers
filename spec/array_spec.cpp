#include "utils.h"

TEST_CASE("array") {
  init_t init(NULL);

  SECTION("array_count handles NULL") {
    int* arr = NULL;
    CHECK(0 == array_count(arr));
  }

  SECTION("array_capacity handles NULL") {
    int* arr = NULL;
    CHECK(0 == array_capacity(arr));
  }

  SECTION("array_free handles NULL") {
    int* arr = NULL;
    array_free(arr, NULL);
    CHECK(arr == NULL);
  }

  SECTION("array_push handles NULL") {
    int* arr = NULL;
    array_push(arr, 10, NULL);
    CHECK(array_count(arr) == 1);
    CHECK(array_capacity(arr) >= 1);
    CHECK(arr[0] == 10);
    array_free(arr, NULL);
  }

  SECTION("array_push inserts at the front") {
    int* arr = NULL;
    array_push(arr, 40, NULL);
    array_push(arr, 41, NULL);
    array_push(arr, 42, NULL);
    CHECK(array_count(arr) == 3);
    CHECK(arr[0] == 40);
    CHECK(arr[1] == 41);
    CHECK(arr[2] == 42);
    array_free(arr, NULL);
  }

  SECTION("array_reserve handles NULL") {
    int* arr = NULL;
    array_reserve(arr, 50, NULL);
    CHECK(array_capacity(arr) == 50);
    CHECK(array_count(arr) == 0);
    CHECK(arr != NULL);
    array_free(arr, NULL);
  }

  SECTION("array_reserve properly grows") {
    int* arr = NULL;
    array_reserve(arr, 50, NULL);
    array_push(arr, 42, NULL);
    array_reserve(arr, 100, NULL);
    CHECK(array_capacity(arr) == 100);
    CHECK(array_count(arr) == 1);
    CHECK(arr[0] == 42);
    CHECK(arr != NULL);
    array_free(arr, NULL);
  }

  SECTION("array_reserve_more handles NULL") {
    int* arr = NULL;
    array_reserve_more(arr, 5, NULL);
    CHECK(array_capacity(arr) == 5);
    CHECK(array_count(arr) == 0);
    CHECK(arr != NULL);
    array_free(arr, NULL);
  }

  SECTION("array_reserve_more properly grows") {
    int* arr = NULL;
    array_reserve_more(arr, 5, NULL);
    array_push(arr, 42, NULL);
    array_reserve_more(arr, 20, NULL);
    CHECK(array_capacity(arr) == 21);
    CHECK(array_count(arr) == 1);
    CHECK(arr[0] == 42);
    CHECK(arr != NULL);
    array_free(arr, NULL);
  }

  SECTION("array_pop reduces the count") {
    int* arr = NULL;
    array_push(arr, 42, NULL);
    array_push(arr, 18, NULL);
    CHECK(array_count(arr) == 2);
    CHECK(arr[0] == 42);
    CHECK(arr[1] == 18);
    array_pop(arr);
    CHECK(array_count(arr) == 1);
    array_free(arr, NULL);
  }

  SECTION("array_unshift handles NULL") {
    int* arr = NULL;
    array_unshift(arr, 42, NULL);
    CHECK(array_count(arr) == 1);
    CHECK(arr[0] == 42);
    array_free(arr, NULL);
  }

  SECTION("array_unshift inserts at the front") {
    int* arr = NULL;
    array_unshift(arr, 40, NULL);
    array_unshift(arr, 41, NULL);
    array_unshift(arr, 42, NULL);
    CHECK(array_count(arr) == 3);
    CHECK(arr[0] == 42);
    CHECK(arr[1] == 41);
    CHECK(arr[2] == 40);
    array_free(arr, NULL);
  }

  SECTION("array_shift removes from the front") {
    int* arr = NULL;
    array_push(arr, 40, NULL);
    array_push(arr, 41, NULL);
    array_push(arr, 42, NULL);
    CHECK(array_count(arr) == 3);
    array_shift(arr);
    CHECK(array_count(arr) == 2);
    CHECK(arr[0] == 41);
    CHECK(arr[1] == 42);
    array_shift(arr);
    CHECK(array_count(arr) == 1);
    CHECK(arr[0] == 42);
    array_free(arr, NULL);
  }

  SECTION("array_push_n handles NULL") {
    int extra[] = {10, 11, 12};
    int* arr = NULL;
    array_push_n(arr, extra, 3, NULL);
    CHECK(array_count(arr) == 3);
    CHECK(arr[0] == 10);
    CHECK(arr[1] == 11);
    CHECK(arr[2] == 12);
    array_free(arr, NULL);
  }

  SECTION("array_push_n correctly adds to the end") {
    int extra[] = {10, 11, 12};
    int* arr = NULL;
    array_push(arr, 0, NULL);
    array_push(arr, 1, NULL);
    array_push(arr, 2, NULL);
    array_push_n(arr, extra, 3, NULL);
    CHECK(array_count(arr) == 6);
    CHECK(arr[0] == 0);
    CHECK(arr[1] == 1);
    CHECK(arr[2] == 2);
    CHECK(arr[3] == 10);
    CHECK(arr[4] == 11);
    CHECK(arr[5] == 12);
    array_free(arr, NULL);
  }

  SECTION("array_pop_n correctly reduces the count") {
    int* arr = NULL;
    array_push(arr, 0, NULL);
    array_push(arr, 1, NULL);
    array_push(arr, 2, NULL);
    array_push(arr, 3, NULL);
    array_push(arr, 4, NULL);
    array_pop_n(arr, 2);
    CHECK(array_count(arr) == 3);
    CHECK(arr[0] == 0);
    CHECK(arr[1] == 1);
    CHECK(arr[2] == 2);
    array_pop_n(arr, 3);
    CHECK(array_count(arr) == 0);
    array_free(arr, NULL);
  }

  SECTION("array_shift_n removes from the front") {
    int* arr = NULL;
    int items[] = {40, 41, 42, 43, 44};
    array_push_n(arr, items, 5, NULL);
    CHECK(array_count(arr) == 5);
    array_shift_n(arr, 2);
    CHECK(array_count(arr) == 3);
    CHECK(arr[0] == 42);
    CHECK(arr[1] == 43);
    CHECK(arr[2] == 44);
    array_shift_n(arr, 3);
    CHECK(array_count(arr) == 0);
    array_free(arr, NULL);
  }

  SECTION("array_unshift_n handles NULL") {
    int extra[] = {10, 11, 12};
    int* arr = NULL;
    array_unshift_n(arr, extra, 3, NULL);
    CHECK(array_count(arr) == 3);
    CHECK(arr[0] == 10);
    CHECK(arr[1] == 11);
    CHECK(arr[2] == 12);
    array_free(arr, NULL);
  }

  SECTION("array_unshift_n correctly adds to the end") {
    int extra[] = {10, 11, 12};
    int* arr = NULL;
    array_push(arr, 0, NULL);
    array_push(arr, 1, NULL);
    array_push(arr, 2, NULL);
    array_unshift_n(arr, extra, 3, NULL);
    CHECK(array_count(arr) == 6);
    CHECK(arr[0] == 10);
    CHECK(arr[1] == 11);
    CHECK(arr[2] == 12);
    CHECK(arr[3] == 0);
    CHECK(arr[4] == 1);
    CHECK(arr[5] == 2);
    array_free(arr, NULL);
  }
}

TEST_CASE("array with custom alloc") {
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
    uint32_t allocator_a = 0;
    uint32_t allocator_b = 0;
    int* arr_a = NULL;
    int* arr_b = NULL;
    array_push(arr_a, 1, &allocator_a);
    CHECK(allocator_a == 1);
    array_push(arr_b, 1, &allocator_b);
    CHECK(allocator_b == 1);
    array_reserve(arr_a, 128, &allocator_a);
    CHECK(allocator_a == 1);
    array_reserve(arr_b, 128, &allocator_b);
    CHECK(allocator_b == 1);
    array_free(arr_a, &allocator_a);
    CHECK(allocator_a == 0);
    array_free(arr_b, &allocator_b);
    CHECK(allocator_b == 0);
  }
}

#ifdef CONTAINERS_CHECK_ENABLED
TEST_CASE("array with checks") {
  containers_lib_config_t config;
  containers_lib_config_init(&config);
  config.assert_failed = [](const char* expression, const char* message, const char* file, int line, const char* func) {
    throw std::runtime_error(message);
  };
  init_t init(&config);

  SECTION("array_pop asserts if the array is empty") {
    int* arr = NULL;
    CHECK_THROWS_WITH(array_pop(arr), "array must contain at least 1 element");
  }

  SECTION("array_first throws if the array is empty") {
    int* arr = NULL;
    CHECK_THROWS_WITH(array_first(arr), "array must contain at least 1 element");
  }

  SECTION("array_last throws if the array is empty") {
    int* arr = NULL;
    CHECK_THROWS_WITH(array_last(arr), "array must contain at least 1 element");
  }

  SECTION("array_shift throws if the array is empty") {
    int* arr = NULL;
    CHECK_THROWS_WITH(array_shift(arr), "array must contain at least 1 element");
  }

  SECTION("array_pop_n throws if the array is too small") {
    int* arr = NULL;
    CHECK_THROWS_WITH(array_pop_n(arr, 1), "array must contain at least 1 element");
    int items[] = {0, 1, 2, 3};
    array_push_n(arr, items, 4, NULL);
    CHECK_THROWS_WITH(array_pop_n(arr, 5), "array must contain at least 5 elements");
    array_free(arr, NULL);
  }

  SECTION("array_shift_n throws if the array is too small") {
    int* arr = NULL;
    CHECK_THROWS_WITH(array_shift_n(arr, 1), "array must contain at least 1 element");
    int items[] = {0, 1, 2, 3};
    array_push_n(arr, items, 4, NULL);
    CHECK_THROWS_WITH(array_shift_n(arr, 5), "array must contain at least 5 elements");
    array_free(arr, NULL);
  }
}
#endif // CONTAINERS_CHECK
