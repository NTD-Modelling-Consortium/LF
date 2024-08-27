#include "Model.hpp"
#include <catch2/catch_all.hpp>
TEST_CASE("Model", "[classic]") {
  SECTION("Model::reduceImportationViaPrevalenceCheck") {

    Model model;

    // test the model.reduceImportationViaPrevalenceCheck function
    // this should return false if reduceImpViaXml = 1 and the time is below
    // switchImportationReducingMethodTime. Otherwise this should be true
    int reduceImpViaXml = 0;
    int t = 0;
    int switchImportationReducingMethodTime = 270;

    REQUIRE(model.reduceImportationViaPrevalenceCheck(
        reduceImpViaXml, t, switchImportationReducingMethodTime));

    t = 270;
    REQUIRE(model.reduceImportationViaPrevalenceCheck(
        reduceImpViaXml, t, switchImportationReducingMethodTime));

    // the next two times the function is called should be false. This is
    // because the reduceImpViaXml variable is set to 1 and the time is before
    // switchImportationReducingMethodTime
    reduceImpViaXml = 1;
    t = 0;
    REQUIRE(!model.reduceImportationViaPrevalenceCheck(
        reduceImpViaXml, t, switchImportationReducingMethodTime));
    t = 269;
    REQUIRE(!model.reduceImportationViaPrevalenceCheck(
        reduceImpViaXml, t, switchImportationReducingMethodTime));

    t = 270;
    // the next should be true as the time is now equal to
    // switchImportationReducingMethodTime
    REQUIRE(model.reduceImportationViaPrevalenceCheck(
        reduceImpViaXml, t, switchImportationReducingMethodTime));
  }
}
