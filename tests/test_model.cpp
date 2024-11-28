#include "Model.hpp"
#include <catch2/catch_all.hpp>
TEST_CASE("Model", "[classic]") {
  SECTION("Model::shouldReduceImportationViaPrevalance") {

    Model model;

    SECTION("Importation not reduced via xml, so any time point should do "
            "reduction via prevalence") {
      REQUIRE(model.shouldReduceImportationViaPrevalance(0, 0, 270));
      REQUIRE(model.shouldReduceImportationViaPrevalance(0, 270, 270));
      REQUIRE(model.shouldReduceImportationViaPrevalance(0, 500, 270));
    }

    SECTION("Importation reduced via xml, so any time point before switch time "
            "should not do reduction via prevalence") {
      REQUIRE_FALSE(model.shouldReduceImportationViaPrevalance(1, 0, 270));
      REQUIRE_FALSE(model.shouldReduceImportationViaPrevalance(1, 269, 270));
    }

    SECTION("Importation reduced via xml, but if at time point at or after "
            "switch time, then should do reduction via prevalence") {
      REQUIRE(model.shouldReduceImportationViaPrevalance(1, 270, 270));
      REQUIRE(model.shouldReduceImportationViaPrevalance(1, 500, 270));
    }

    int t = 1;

    SECTION("Reduce importation rate via xml, this means that the test should "
            "be true when we run !function we are testing") {
      REQUIRE(!model.shouldReduceImportationViaPrevalance(1, 0, 270));
    }

    SECTION("Repeat previous test, but also test whether time is divisible by "
            "12, and as it isn't, this should be false") {
      REQUIRE_FALSE((!model.shouldReduceImportationViaPrevalance(1, t, 270) &&
                     (t % 12 == 0)));
    }
  }

  SECTION("Model::multiplierForCoverage") {

    Model model;

    SECTION("coverage multiplier should be unchanged when we don't remove "
            "coverage reduction ") {
      int removeCoverageReduction = 0;
      int removeCoverageReductionTime = 276;
      int graduallyRemoveCoverageReduction = 0;
      double cov_prop = 0.6;
      REQUIRE(model.multiplierForCoverage(0, cov_prop, removeCoverageReduction,
                                          removeCoverageReductionTime,
                                          graduallyRemoveCoverageReduction) ==
              0.4);
    }
  }
}
