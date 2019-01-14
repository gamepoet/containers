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
}
