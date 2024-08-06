#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

TEST_CASE("Test positives", "[classic]") {
  SECTION("Test all up to 10") { REQUIRE(1 == 1); }
}