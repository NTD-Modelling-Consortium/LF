#include "Host.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Host", "[classic]") {
  SECTION("Host::restore") {
    // Initialize a population of hosts
    int size = 10;
    double TotalBiteRisk = 0.0;
    Host host_pop[size];

    // initialise population and set pTreat value
    for (int i = 0; i < size; ++i) {
      host_pop[i].initialise(1.0, 100, 0.2, &TotalBiteRisk, 0.1, 0.1, 0.0);
      host_pop[i].pTreat = (1.0 / i);
    }

    // Save the state of each host
    //hostState currentState[size];
    std::vector<hostState> currentState;
    currentState.resize(size,
                           {0, 0, 0, 0.0, 0.0, 0.0, 0, 0,
                            0.0}); // WM, WF,totalWorms, totalWormYears,
                                   // M,biteRisk,age,monthSinceTreated, pTreat
    for (int i = 0; i < size; ++i) {
      currentState[i] = host_pop[i];
    }

    // change the values in some fields, so that we know that the tested code is
    // doing something
    for (int i = 0; i < size; ++i) {
      host_pop[i].WM = 10;
      host_pop[i].biteRisk = 1.0;
      host_pop[i].pTreat = 1.0;
    }

    // Restore the hosts from the saved state
    for (int i = 0; i < size; ++i) {
      host_pop[i].restore(currentState[i]);
    }

    // check that each host's state matches the saved state
    for (int i = 0; i < size; ++i) {
      REQUIRE(host_pop[i].WM == currentState[i].WM);
      REQUIRE(host_pop[i].WF == currentState[i].WF);
      REQUIRE(host_pop[i].totalWorms == currentState[i].totalWorms);
      REQUIRE(host_pop[i].totalWormYears == currentState[i].totalWormYears);
      REQUIRE(host_pop[i].M == currentState[i].M);
      REQUIRE(host_pop[i].biteRisk == currentState[i].biteRisk);
      REQUIRE(host_pop[i].age == currentState[i].age);
      REQUIRE(host_pop[i].monthsSinceTreated ==
              currentState[i].monthsSinceTreated);
      REQUIRE(host_pop[i].hydroMult == currentState[i].hydroMult);
      REQUIRE(host_pop[i].lymphoMult == currentState[i].lymphoMult);
      REQUIRE(host_pop[i].sex == currentState[i].sex);
      REQUIRE(host_pop[i].pTreat == 1.0 / i);
      REQUIRE(host_pop[i].pTreat == currentState[i].pTreat);
    }
  }
}
