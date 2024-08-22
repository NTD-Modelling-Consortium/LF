#include <catch2/catch_all.hpp>
#include "Host.hpp"

TEST_CASE("Host", "[classic]")
{
    SECTION("Host::restore")
    {
        Host host;
        hostState state;
        host.restore(state);
    }
}
