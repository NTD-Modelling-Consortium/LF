//
//  Model.hpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp

#include <fstream>
#include <random>
#include <vector>

#include "Output.hpp"

class Scenario;
class ScenariosList;
class Population;
class Vector;
class Worm;
class PrevalenceEvent;

class Model {

public:
  void runScenarios(ScenariosList &scenarios, Population &popln,
                    Vector &vectors, Worm &worms, int replicates,
                    double timestep, int index, int outputEndgame,
                    int outputEndgameDate, bool outputNTDMC,
                    std::optional<int> outputNTDMCDate, int reduceImpViaXml,
                    std::string randParamsfile, std::string RandomSeedFile,
                    std::string RandomCovPropFile, std::string opDir);
  bool
  shouldReduceImportationViaPrevalance(int t, int reduceImpViaXml,
                                       int switchImportationReducingMethodTime);

  double multiplierForCoverage(int t, double cov_prop,
                               int removeCoverageReduction,
                               int removeCoverageReductionTime,
                               int graduallyRemoveCoverageReduction);

protected:
  void burnIn(Population &popln, Vector &vectors, const Worm &worms,
              Output &currentOutput, PrevalenceEvent *pe);
  void evolveAndSave(int y, Population &popln, Vector &vectors, Worm &worms,
                     Scenario &sc, Output &currentOutput, int rep,
                     std::vector<double> &k_vals,
                     std::vector<double> &v_to_h_vals, int updateParams,
                     int outputEndgame, int outputEndgameDate, bool outputNTDMC,
                     int outputNTDMCDate, int reduceImpViaXml,
                     std::string opDir, double cov_prop);
  void getRandomParameters(int index, std::vector<double> &k_vals,
                           std::vector<double> &v_to_h_vals,
                           std::vector<double> &aImp_vals,
                           std::vector<double> &wPropMDA, unsigned replicates,
                           std::string fname);
  void getRandomParametersMultiplePerLine(
      int index, std::vector<double> &k_vals, std::vector<double> &v_to_h_vals,
      std::vector<double> &aImp_vals, std::vector<double> &wProp_vals,
      unsigned replicates, std::string fname);
  void readSeedsFromFile(std::vector<unsigned long int> &seeds,
                         unsigned replicates, std::string fname);
  void readCovPropFromFile(std::vector<double> &cov_props, unsigned replicates,
                           std::string fname);
  void ProcessLine(const std::string &line, std::vector<double> &k_vals,
                   std::vector<double> &v_to_h_vals,
                   std::vector<double> &aImp_vals,
                   std::vector<double> &wProp_vals);

  int currentMonth;
  double dt;
  std::vector<std::string> printSeedName() const;
};

#endif /* Model_hpp */
