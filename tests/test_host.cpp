#include "Host.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Host", "[classic]") {
  SECTION("Host::restore") {
    Host host;
    hostState state;
    host.restore(state);
  }
}
