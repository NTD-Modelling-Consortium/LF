//
//  Scenario.hpp
//  transfil
//
//  Created by Paul Brown on 02/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Scenario_hpp
#define Scenario_hpp

#include "BedNetEvent.hpp"
#include "ImportationRateEvent.hpp"
#include "MDAEvent.hpp"
#include "Population.hpp"
#include "PrevalenceEvent.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Output;
class Population;

// class to represent a single scenario

class Scenario {

public:
  Scenario(int numMonths, int sMonth) : startMonth(sMonth) {

    outputDue.assign(numMonths, NULL);
    mdaDue.assign(numMonths, NULL);
    aImpUpdate.assign(numMonths, NULL);

    name = "Scenario";
  };

  Scenario(int numMonths, int sMonth, std::string nm)
      : startMonth(sMonth), name(nm) {

    outputDue.assign(numMonths, NULL);
    mdaDue.assign(numMonths, NULL);
    aImpUpdate.assign(numMonths, NULL);
  };

  Scenario(int numMonths, int sMonth, std::string nm,
           std::vector<BedNetEvent> &bn, std::vector<MDAEvent> &mda,
           std::vector<PrevalenceEvent> &prevOut,
           std::vector<ImportationRateEvent> &aImpF)
      : startMonth(sMonth), name(nm) {

    // default bednet coverage is none
    // default mda is none
    // default output is every jan

    outputDue.assign(numMonths, NULL);
    mdaDue.assign(numMonths, NULL);
    bedNetCoverage.assign(numMonths, NULL); // default is no bednets
    aImpUpdate.assign(numMonths, NULL);

    // sort
    bedNets = bn;
    mdaEvents = mda;
    outputEvents = prevOut;
    aImpFactors = aImpF;

    std::sort(bedNets.begin(), bedNets.end(),
              BedNetEvent::sort); // overloaded <opertor does compare
    std::sort(mdaEvents.begin(), mdaEvents.end(), MDAEvent::sort);
    std::sort(outputEvents.begin(), outputEvents.end(), PrevalenceEvent::sort);
    std::sort(aImpFactors.begin(), aImpFactors.end(),
              ImportationRateEvent::sort);

    for (unsigned month = 0; month < outputEvents.size(); month++) {

      int prevMonth = outputEvents[month].getMonth();
      if (prevMonth < numMonths)
        outputDue[prevMonth] =
            &(outputEvents[month]); // one entry fo revery month, null pointer
                                    // if outpu tnot needed, otherwise pointer
                                    // to a structure giving the details
    }

    // record age range of extea output for creating file names. These will be
    // the same for all outputs so just use first
    minAgeExtra = outputEvents[0].getMinAgeExtra();
    maxAgeExtra = outputEvents[0].getMaxAgeExtra();
    requiresExtra =
        ((minAgeExtra > 0) || (maxAgeExtra < Population::getMaxAge()));

    // same for prevalence method

    MFNeeded = outputEvents[0].getMethod("mf");
    ICNeeded = outputEvents[0].getMethod("ic");
    WCNeeded = outputEvents[0].getMethod("wc");

    for (unsigned month = 0; month < mdaEvents.size(); month++) {

      int mdaMonth = mdaEvents[month].getMonth();
      if (mdaMonth < numMonths)
        mdaDue[mdaMonth] = &(mdaEvents[month]);
    }

    // bednets will always be defined at month zero
    for (unsigned month = 0; month < bedNets.size(); month++) {

      int bednetMonth = bedNets[month].getMonth();
      if (bednetMonth < numMonths)
        bedNetCoverage[bednetMonth] = &(bedNets[month]);
    }

    if (!bedNetCoverage[0]) {
      std::cout << "Error in Scenario::Scenario, scenario " << name
                << ". Bednet coverage not completely defined." << std::endl;
      exit(1);
    }

    for (unsigned month = 0; month < aImpFactors.size(); month++) {

      int aImpMonth = aImpFactors[month].getMonth();
      if (aImpMonth < numMonths)
        aImpUpdate[aImpMonth] = &(aImpFactors[month]);
    }

    if (!aImpUpdate[0]) {
      std::cout << "Error in Scenario::Scenario, scenario " << name
                << ". Importation rate not completely defined." << std::endl;
      exit(1);
    }
  }
  /*
     ~Scenario() {

         if (popFiles != NULL)
           delete[] popFiles;
     }
 */

  std::string getName() const { return name; }

  void updateImportationRate(Population &popln, int mont);
  void updateBedNetCoverage(Population &popln, int month);

  MDAEvent *treatmentDue(int month) const;
  PrevalenceEvent *prevalenceDue(int month) const;

  void setMonthsToSave(std::vector<int> &mnths);

  double getImportationFactor(int month) const;
  BedNetEvent *getBedNetCoverage(int month) const;
  int getStartMonth() const;
  int getNumMonthsToSave() const;
  int getMonthToSave(int idx) const;
  bool requiresIC() const;
  bool requiresMF() const;
  bool requiresWC() const;

  void openFileandPrintHeadings(std::string region, Output &results);
  void printColumnTitles(std::ofstream &of, Output &results,
                         bool extra = false) const;
  void printResults(int repnum, Output &results, Population &popln);
  void printReplicate(std::ofstream &of, Output &results, int repnum,
                      Population &popln) const;
  void closeFile();
  std::string getName();

  void InitIHMEData(int rep, std::string folder);
  void InitNTDMCData(int rep, std::string folder);
  void InitPreTASData(int rep, std::string folder);
  void InitTASData(int rep, std::string folder);
  void writePrevByAge(Population &popln, int t, int rep, std::string folder);
  void writeRoadmapTarget(Population &popln, int t, int rep, int DoMDA,
                          int TAS_Pass, int neededTASPass, std::string folder);
  void writeNumberByAge(Population &popln, int t, int rep, std::string folder,
                        std::string surveyType);
  void writeSequelaeByAge(Population &popln, int t, int LymphodemaTotalWorms,
                          double LymphodemaShape, int HydroceleTotalWorms,
                          double HydroceleShape, int rep, std::string folder);
  void writeMDADataAllTreated(int t, int *numTreat, int *numHosts, int maxAge,
                              int rep, std::string type, std::string folder);
  void writePreTAS(int t, int *numSurvey, int maxAge, int rep,
                   std::string folderName);
  void writeTAS(int t, int *numSurvey, int maxAge, int rep, std::string folder);
  void writeSurveyByAge(Population &popln, int t, int preTAS_Pass, int TAS_Pass,
                        int rep, std::string folder);
  void writeEmptySurvey(int year, int maxAge, int rep, std::string surveyType,
                        std::string folder);
  void writeIncidence(int t, int *numSurvey, int maxAge, int rep,
                      std::string folder);

protected:
  int startMonth;

  std::vector<BedNetEvent> bedNets;
  std::vector<BedNetEvent *> bedNetCoverage;
  std::vector<MDAEvent> mdaEvents;
  std::vector<MDAEvent *> mdaDue;
  std::vector<PrevalenceEvent> outputEvents;
  std::vector<PrevalenceEvent *> outputDue;
  std::vector<ImportationRateEvent> aImpFactors;
  std::vector<ImportationRateEvent *> aImpUpdate;

  std::vector<int> monthsToSave;

  std::string name;
  std::ofstream myFileMF, myFileIC, myFileWC, myFileL3;
  std::ofstream myFileExtraMF, myFileExtraIC, myFileExtraWC;

  std::ofstream *popFiles = NULL;
  const std::vector<int> popAgeRanges = {5};

  int minAgeExtra;
  int maxAgeExtra;
  bool requiresExtra;
  bool MFNeeded;
  bool ICNeeded;
  bool WCNeeded;
};

#endif /* Scenario_hpp */
