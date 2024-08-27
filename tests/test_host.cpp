#include "Host.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Host", "[classic]") {
  SECTION("Host::restore") {
    // Initialize a population of hosts
    int size = 1;
    double TotalBiteRisk = 0.0;
    Host host_pop;

    // initialise population and set pTreat value

    host_pop.initialise(1.0, 100, 0.2, &TotalBiteRisk, 0.1, 0.1, 0.0);
    host_pop.WM = 1;
    host_pop.WF = 2;
    host_pop.totalWorms = 3;
    host_pop.totalWormYears = 4.1;
    host_pop.M = 5.0;
    host_pop.biteRisk = 6.0;
    host_pop.age = 7.0;
    host_pop.monthsSinceTreated = 8;
    host_pop.hydroMult = 9.0;
    host_pop.lymphoMult = 10.0;
    host_pop.sex = 0;
    host_pop.pTreat = 3.1415926535;

    // Save the state of each host
    // hostState currentState[size];
    hostState currentState;
    currentState = host_pop;

    // change the values in some fields, so that we know that the tested code is
    // doing something

    host_pop.WM = 0;
    host_pop.WF = 0;
    host_pop.totalWorms = 0;
    host_pop.totalWormYears = 0.0;
    host_pop.M = 0.0;
    host_pop.biteRisk = 0.0;
    host_pop.age = 0.0;
    host_pop.monthsSinceTreated = 0;
    host_pop.hydroMult = 0.0;
    host_pop.lymphoMult = 0.0;
    host_pop.sex = 1;
    host_pop.pTreat = 0.0;

    // Restore the hosts from the saved state

    host_pop.restore(currentState);

    // check that each host's state matches the saved state

    REQUIRE(host_pop.WM == 1);
    REQUIRE(host_pop.WF == 2);
    REQUIRE(host_pop.totalWorms == 3);
    REQUIRE(host_pop.totalWormYears == 4.1);
    REQUIRE(host_pop.M == 5.0);
    REQUIRE(host_pop.biteRisk == 6.0);
    REQUIRE(host_pop.age == 7.0);
    REQUIRE(host_pop.monthsSinceTreated == 8);
    REQUIRE(host_pop.hydroMult == 9.0);
    REQUIRE(host_pop.lymphoMult == 10.0);
    REQUIRE(host_pop.sex == 0);
    REQUIRE(host_pop.pTreat == 3.1415926535);
  }
}
