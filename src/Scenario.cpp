//
//  Scenario.cpp
//  transfil
//
//  Created by Paul Brown on 02/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "Scenario.hpp"
#include "Output.hpp"
#include <cassert>
#include <filesystem>
#include <sstream>
#include <string>
#include <sys/stat.h>
namespace fs = std::filesystem;

bool IsPathExist(const std::string &s) {
  struct stat buffer;
  return (stat(s.c_str(), &buffer) == 0);
}

void Scenario::updateBedNetCoverage(Population &popln, int month) {

  popln.updateBedNetCoverage(getBedNetCoverage(month));
}

void Scenario::updateImportationRate(Population &popln, int month) {

  // This has effect on Vector::L3 below and also on host::react

  popln.updateImportationRate(getImportationFactor(month));
}

BedNetEvent *Scenario::getBedNetCoverage(int month) const {

  // If NULL pointer, go back to the last month it was defined.
  // bedNetCoverage[0] guaranteed not to be null
  BedNetEvent *bn = NULL;
  while (!(bn = bedNetCoverage[month]))
    month--;

  return bn; // never NULL
}

double Scenario::getImportationFactor(int month) const {

  ImportationRateEvent *aImpF = NULL;
  while (!(aImpF = aImpUpdate[month]))
    month--;

  return aImpF->getFactor();
}

MDAEvent *Scenario::treatmentDue(int month) const {

  // cale each month by Model::evovleAndSAve()
  // month is zero based month number from start of simulation

  return mdaDue[month]; // NULL if not due
}

PrevalenceEvent *Scenario::prevalenceDue(int month) const {

  return outputDue[month]; // NULL if not due
}

void Scenario::setMonthsToSave(std::vector<int> &mnts) { monthsToSave = mnts; }

int Scenario::getStartMonth() const { return startMonth; }

int Scenario::getNumMonthsToSave() const {

  return int(monthsToSave.size()); // allows us to go back in time to re-use
                                   // data
}

int Scenario::getMonthToSave(int idx) const { return monthsToSave[idx]; }

void Scenario::openFileandPrintHeadings(std::string region, Output &results) {

  std::string fname;

  popFiles = new std::ofstream[popAgeRanges.size()];

  // pop size file(s)
  for (int i = 0; i < popAgeRanges.size(); ++i) {

    fname = region + "_" + name + "_Pop_" + std::to_string(popAgeRanges[i]) +
            ".txt";

    popFiles[i].open(fname, std::ios_base::out);
    if (!popFiles[i].is_open()) {

      std::cout << "Error writing results to file " << fname << std::endl;
      exit(1);
    }

    popFiles[i] << name << "\tHosts in population aged ";
    if (i == popAgeRanges.size() - 1) // last age range
      popFiles[i] << std::to_string(popAgeRanges[i]) << " years and above"
                  << std::endl;
    else
      popFiles[i] << "between" << std::to_string(popAgeRanges[i]) << " and "
                  << std::to_string(popAgeRanges[i + 1] - 1) << " years"
                  << std::endl;
  }

  if (ICNeeded) {

    fname = region + "_" + name + "_IC.txt";
    myFileIC.open(fname, std::ios_base::out);
    if (!myFileIC.is_open()) {

      std::cout << "Error writing results to file " << fname << std::endl;
      exit(1);
    }
    myFileIC << name << "\tPevalence calculated using IC method" << std::endl;
  }

  if (MFNeeded) {

    fname = region + "_" + name + "_MF.txt";
    myFileMF.open(fname, std::ios_base::out);
    if (!myFileMF.is_open()) {

      std::cout << "Error writing results to file " << fname << std::endl;
      exit(1);
    }
    myFileMF << name << "\tPevalence calculated using MF method" << std::endl;
  }

  if (WCNeeded) {

    fname = region + "_" + name + "_WC.txt";
    myFileWC.open(fname, std::ios_base::out);
    if (!myFileWC.is_open()) {

      std::cout << "Error writing results to file " << fname << std::endl;
      exit(1);
    }
    myFileWC << name << "\tPevalence calculated using Worm count method"
             << std::endl;
  }

  // do we need extra file for extra age-restriced outputs?
  if (requiresExtra) {

    std::string fnameExtra = region + "_" + name + "_age_";
    fnameExtra += std::to_string(minAgeExtra);
    fnameExtra += "_";
    fnameExtra += std::to_string(maxAgeExtra);

    if (ICNeeded) {

      fname = fnameExtra + "_IC.txt";
      myFileExtraIC.open(fname, std::ios_base::out);
      if (!myFileExtraIC.is_open()) {

        std::cout << "Error writing results to file " << fname << std::endl;
        exit(1);
      }
      myFileExtraIC
          << name
          << "\tPevalence calculated using IC method\tHosts aged between "
          << minAgeExtra << " and " << maxAgeExtra << " years" << std::endl;
    }

    if (MFNeeded) {

      fname = fnameExtra + "_MF.txt";

      myFileExtraMF.open(fname, std::ios_base::out);
      if (!myFileExtraMF.is_open()) {

        std::cout << "Error writing results to file " << fname << std::endl;
        exit(1);
      }
      myFileExtraMF
          << name
          << "\tPevalence calculated using MF method\tHosts aged between "
          << minAgeExtra << " and " << maxAgeExtra << " years" << std::endl;
    }

    if (WCNeeded) {

      fname = fnameExtra + "_WC.txt";

      myFileExtraWC.open(fname, std::ios_base::out);
      if (!myFileExtraWC.is_open()) {

        std::cout << "Error writing results to file " << fname << std::endl;
        exit(1);
      }
      myFileExtraWC << name
                    << "\tPevalence calculated using Worm count method\tHosts "
                       "aged between "
                    << minAgeExtra << " and " << maxAgeExtra << " years"
                    << std::endl;
    }
  }
}

void Scenario::printResults(int repnum, Output &results, Population &popln) {

  // caled at the end of each replicate

  if (ICNeeded) {
    if (!repnum)
      printColumnTitles(myFileIC, results);
    printReplicate(myFileIC, results, repnum,
                   popln); // print randaom values used in this replicate
  }
  if (MFNeeded) {
    if (!repnum)
      printColumnTitles(myFileMF, results);
    printReplicate(myFileMF, results, repnum, popln);
  }
  if (WCNeeded) {
    if (!repnum)
      printColumnTitles(myFileWC, results);
    printReplicate(myFileWC, results, repnum, popln);
  }

  for (int i = 0; i < popAgeRanges.size(); i++) {
    if (!repnum)
      printColumnTitles(popFiles[i], results);
    printReplicate(popFiles[i], results, repnum, popln);
  }

  if (requiresExtra) {

    if (ICNeeded) {
      if (!repnum)
        printColumnTitles(myFileExtraIC, results, true);
      printReplicate(myFileExtraIC, results, repnum, popln);
    }
    if (MFNeeded) {
      if (!repnum)
        printColumnTitles(myFileExtraMF, results, true);
      printReplicate(myFileExtraMF, results, repnum, popln);
    }
    if (WCNeeded) {
      if (!repnum)
        printColumnTitles(myFileExtraWC, results, true);
      printReplicate(myFileExtraWC, results, repnum, popln);
    }
  }

  // print prevalence for each month with either mda or output (see
  // Model::evolveAndSave())

  for (int m = 0; m < results.getSize(); m++) {

    // For each month

    RecordedPrevalence *prevalence = results.printPrevalence(m);
    // this will be null if output month exists only due to mda

    // pop size

    for (int i = 0; i < popAgeRanges.size(); i++) {

      if (prevalence) {

        int lower = popAgeRanges[i];
        int upper =
            (i < (popAgeRanges.size() - 1)) ? popAgeRanges[i + 1] - 1 : -1;
        popFiles[i] << "\t"
                    << prevalence->getAgeInRange(lower,
                                                 upper); // range is inclusive

      } else
        popFiles[i] << "\t "; // placeholder, just mda needed for this month
    }

    if (ICNeeded) {
      if (prevalence)
        myFileIC << "\t" << prevalence->IC;
      else
        myFileIC << "\t"
                 << " "; // placeholder, just mda needed for this month
    }

    if (MFNeeded) {
      if (prevalence)
        myFileMF << "\t" << prevalence->MF;
      else
        myFileMF << "\t"
                 << " ";
    }

    if (WCNeeded) {
      if (prevalence)
        myFileWC << "\t" << prevalence->WC;
      else
        myFileWC << "\t"
                 << " ";
    }

    if (requiresExtra) {

      if (ICNeeded) {

        if (prevalence)
          myFileExtraIC << "\t" << prevalence->ICRestrictedAge;
        else
          myFileExtraIC << "\t"
                        << " "; // placeholder, just mda needed for this month
      }

      if (MFNeeded) {

        if (prevalence)
          myFileExtraMF << "\t" << prevalence->MFRestrictedAge;
        else
          myFileExtraMF << "\t"
                        << " ";
      }

      if (WCNeeded) {

        if (prevalence)
          myFileExtraWC << "\t" << prevalence->WCRestrictedAge;
        else
          myFileExtraWC << "\t"
                        << " ";
      }
    }
  }

  if (ICNeeded) {
    myFileIC << "\t" << popln.totMDAs;
    myFileIC << "\t" << popln.post2020MDAs;
    myFileIC << "\t" << popln.numPreTASSurveys;
    myFileIC << "\t" << popln.numTASSurveys;
    myFileIC << "\t" << popln.t_TAS_Pass;
    myFileIC << std::endl;
  }

  if (MFNeeded) {
    myFileMF << "\t" << popln.totMDAs;
    myFileMF << "\t" << popln.post2020MDAs;
    myFileMF << "\t" << popln.numPreTASSurveys;
    myFileMF << "\t" << popln.numTASSurveys;
    myFileMF << "\t" << popln.t_TAS_Pass;
    myFileMF << std::endl;
  }

  if (WCNeeded) {
    myFileWC << "\t" << popln.totMDAs;
    myFileWC << "\t" << popln.post2020MDAs;
    myFileWC << "\t" << popln.numPreTASSurveys;
    myFileWC << "\t" << popln.numTASSurveys;
    myFileWC << "\t" << popln.t_TAS_Pass;
    myFileWC << std::endl;
  }

  if (requiresExtra) {
    if (ICNeeded)
      myFileExtraIC << std::endl;
    if (MFNeeded)
      myFileExtraMF << std::endl;
    if (WCNeeded)
      myFileExtraWC << std::endl;
  }

  for (int i = 0; i < popAgeRanges.size(); i++)
    popFiles[i] << std::endl;
}

void Scenario::printColumnTitles(std::ofstream &of, Output &results,
                                 bool extra) const {

  std::string indent(results.getNumRandomVars() + 2, '\t');

  // print headers for every month with either prevalenc or mda

  of << indent << "Year Index";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printYearIndex(m);
  of << std::endl;
  of << indent << "Month Index";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printMonthIndex(m);
  of << std::endl;

  of << indent << "Date";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printDate(m);
  of << std::endl;

  of << indent << "Bed net coverage";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printBedNetCoverage(m);
  of << std::endl;
  of << indent << "Bed net systemtic compliance";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printBednetSysComp(m);
  of << std::endl;

  of << indent << "Importation rate factor";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printAImpFactor(m);
  of << std::endl;

  of << indent << "MDA coverage";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printMDACoverage(m);
  of << std::endl;

  of << indent << "MDA type";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printMDAType(m);
  of << std::endl;

  of << indent << "MDA systematic compliance";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.printMDASysComp(m);
  of << std::endl;

  of << indent << "Minimum age to receive drugs";
  for (int m = 0; m < results.getSize(); m++)
    of << "\t" << results.getMinAgeForTreatment(m);
  of << std::endl;

  if (extra) {
    of << indent << "Age range for prevalance calculation";
    for (int m = 0; m < results.getSize(); m++)
      of << "\t" << minAgeExtra << " to " << maxAgeExtra;
    of << std::endl;

  } else {

    of << indent << "Minimum age for prevalence calculation";
    for (int m = 0; m < results.getSize(); m++)
      of << "\t" << results.getMinAgeForPrevalence(m);
    of << std::endl;
  }

  of << "ID";

  for (int i = 0; i < results.getNumRandomVars(); i++)
    of << "\t" << results.getRandomVarNames(i);
  of << std::endl;
}

void Scenario::printReplicate(std::ofstream &of, Output &results, int repnum,
                              Population &popln) const {

  of << (repnum + 1);
  of << "\t" << results.getSeedValue();
  for (int i = 0; i < results.getNumRandomVars(); i++) {

    if (results.getRandomVarValues(i) >= 0.0) {
      of << "\t" << results.getRandomVarValues(i);
    }

    else
      of << "\t"
         << "na"; // this value wasn't used
  }

  of << "\t";
}

void Scenario::closeFile() {

  if (ICNeeded)
    myFileIC.close();
  if (MFNeeded)
    myFileMF.close();
  if (WCNeeded)
    myFileWC.close();

  if (requiresExtra) {

    if (ICNeeded)
      myFileExtraIC.close();
    if (MFNeeded)
      myFileExtraMF.close();
    if (WCNeeded)
      myFileExtraWC.close();
  }

  for (int i = 0; i < popAgeRanges.size(); i++)
    popFiles[i].close();

  delete[] popFiles;
}

bool Scenario::requiresIC() const { return ICNeeded; }

bool Scenario::requiresMF() const { return MFNeeded; }

bool Scenario::requiresWC() const { return WCNeeded; }

void Scenario::InitIHMEData(int rep, std::string folder) {
  // get mf prevalence

  struct stat buffer;

  std::string fname;
  std::string fname2;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
          "_rep_" + rep1 + ".csv";
  fname2 = folder + "/IHME_scen" + fol_n + "/" + name;
  if (stat(fname2.c_str(), &buffer) != 0) {
    fs::create_directories(fname2);
  }
  std::ofstream outfile;
  outfile.open(fname);
  if (rep == 0) {
    outfile << "espen_loc"
            << ","
            << "year_id"
            << ","
            << "age_start"
            << ","
            << "age_end"
            << ","
            << "measure"
            << ","
            << "draw_0"
            << "\n";
  } else {
    outfile << "draw_0"
            << "\n";
  }

  outfile.close();
}

void Scenario::InitPreTASData(int rep, std::string folder) {
  // get mf prevalence

  struct stat buffer;

  std::string fname;
  std::string fname2;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/PreTAS_scen" + name +
          "_rep_" + rep1 + ".csv";
  fname2 = folder + "/IHME_scen" + fol_n + "/" + name;
  if (stat(fname2.c_str(), &buffer) != 0) {
    fs::create_directories(fname2);
  }
  std::ofstream outfile;
  outfile.open(fname);
  if (rep == 0) {
    outfile << "espen_loc"
            << ","
            << "year_id"
            << ","
            << "age_start"
            << ","
            << "age_end"
            << ","
            << "measure"
            << ","
            << "draw_0"
            << "\n";
  } else {
    outfile << "draw_0"
            << "\n";
  }

  outfile.close();
}

void Scenario::InitTASData(int rep, std::string folder) {
  // get mf prevalence

  struct stat buffer;

  std::string fname;
  std::string fname2;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/TAS_scen" + name +
          "_rep_" + rep1 + ".csv";
  fname2 = folder + "/IHME_scen" + fol_n + "/" + name;
  if (stat(fname2.c_str(), &buffer) != 0) {
    fs::create_directories(fname2);
  }
  std::ofstream outfile;
  outfile.open(fname);
  if (rep == 0) {
    outfile << "espen_loc"
            << ","
            << "year_id"
            << ","
            << "age_start"
            << ","
            << "age_end"
            << ","
            << "measure"
            << ","
            << "draw_0"
            << "\n";
  } else {
    outfile << "draw_0"
            << "\n";
  }

  outfile.close();
}

void Scenario::writePrevByAge(Population &popln, int t, int rep,
                              std::string folder) {
  // get mf prevalence
  std::ofstream outfile;
  int maxAge = popln.getMaxAge();
  std::string fname;
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  std::string rep1 = std::to_string(rep);
  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
          "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;
  outfile.open(fname, std::ios::app);

  bool sample = true;
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "prevalence"
              << "," << popln.getMFPrevByAge(j, j + 1, sample) << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << popln.getMFPrevByAge(j, j + 1, sample) << "\n";
    }
  }

  outfile.close();
}

void Scenario::writeRoadmapTarget(Population &popln, int t, int rep, int DoMDA,
                                  int TAS_Pass, int neededTASPass,
                                  std::string folder) {
  // we want to write whether the population has reached the 2030 roadmap target
  // for each year for LF this is to have microfilaria prevalence below 1% in
  // people 5 years and older we also write the mf prevalence for people 5 years
  // and older, whether we have ceased MDA or not and whether we have achieved
  // eliminated the disease as a public health problem, which is done when we
  // have passed the TAS survey as many times as stated by neededTASPass. This
  // is all done so that there are some easy to use values for each year that
  // can be used for making plots.
  std::ofstream outfile;
  int maxAge = popln.getMaxAge();
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  std::string rep1 = std::to_string(rep);
  std::string fname = folder + "/NTDMC_scen" + fol_n + "/" + name +
                      "/NTDMC_scen" + name + "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;
  outfile.open(fname, std::ios::app);
  bool sample = true;
  float mfprevSample = popln.getMFPrevByAge(5, maxAge, sample);
  float ICprevSample = popln.getICPrevForOutput(sample);

  sample = false;
  float mfprevTrue = popln.getMFPrevByAge(5, maxAge, sample);
  float ICprevTrue = popln.getICPrevForOutput(sample);

  int roadmapTargetMet = mfprevSample <= 0.01 ? 1 : 0;
  int achieveEPHP = TAS_Pass == neededTASPass ? 1 : 0;

  if (rep == 0) {
    outfile << name << "," << year << "," << 5 << "," << maxAge << ","
            << "sampled mf prevalence (all pop)"
            << "," << mfprevSample << "\n";
    outfile << name << "," << year << "," << 5 << "," << maxAge << ","
            << "true mf prevalence (all pop)"
            << "," << mfprevTrue << "\n";
    outfile << name << "," << year << "," << 6 << "," << 7 << ","
            << "sampled IC prevalence (all pop)"
            << "," << ICprevSample << "\n";
    outfile << name << "," << year << "," << 6 << "," << 7 << ","
            << "true IC prevalence (all pop)"
            << "," << ICprevTrue << "\n";
    outfile << name << "," << year << "," << 5 << "," << maxAge << ","
            << "metRoadmapTarget"
            << "," << roadmapTargetMet << "\n";
    outfile << name << "," << year << ","
            << "None"
            << ","
            << "None"
            << ","
            << "MDA ceased"
            << "," << 1 - DoMDA << "\n";
    outfile << name << "," << year << ","
            << "None"
            << ","
            << "None"
            << ","
            << "achieve EPHP"
            << "," << achieveEPHP << "\n";
  } else {
    outfile << mfprevSample << "\n";
    outfile << mfprevTrue << "\n";
    outfile << ICprevSample << "\n";
    outfile << ICprevTrue << "\n";
    outfile << roadmapTargetMet << "\n";
    outfile << 1 - DoMDA << "\n";
    outfile << achieveEPHP << "\n";
  }

  outfile.close();
}

void Scenario::writeIncidence(int t, int *incidence, int maxAge, int rep,
                              std::string folder) {
  std::ofstream outfile;
  std::string fname;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);

  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
          "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Unable to open file " << fname << " for writing."
              << std::endl;
    return; // Or handle the error appropriately
  }
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "Incidence ," << incidence[j] << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << incidence[j] << "\n";
    }
  }

  outfile.close();
}

void Scenario::writeNumberByAge(Population &popln, int t, int rep,
                                std::string folder, std::string surveyType) {
  // get mf prevalence
  std::ofstream outfile;
  int maxAge = popln.getMaxAge();
  std::string fname;
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  std::string rep1 = std::to_string(rep);
  std::string entry = "number";
  if (surveyType == "PreTAS survey") {
    fname = folder + "/IHME_scen" + fol_n + "/" + name + "/PreTAS_scen" + name +
            "_rep_" + rep1 + ".csv";
    entry = "Pre TAS number";
  } else if (surveyType == "TAS survey") {
    fname = folder + "/IHME_scen" + fol_n + "/" + name + "/TAS_scen" + name +
            "_rep_" + rep1 + ".csv";
    entry = "TAS number";
  } else {
    fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
            "_rep_" + rep1 + ".csv";
    entry = "number";
  }

  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << "," << entry
              << "," << popln.getNumberByAge(j, j + 1) << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << popln.getNumberByAge(j, j + 1) << "\n";
    }
  }

  outfile.close();
}

void Scenario::writeSequelaeByAge(Population &popln, int t,
                                  int LymphodemaTotalWorms,
                                  double LymphodemaShape,
                                  int HydroceleTotalWorms,
                                  double HydroceleShape, int rep,
                                  std::string folder) {
  // get mf prevalence
  std::ofstream outfile;
  int maxAge = popln.getMaxAge();
  std::string fname;

  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  std::string rep1 = std::to_string(rep);
  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
          "_rep_" + rep1 + ".csv";

  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "Hydrocele"
              << ","
              << popln.HydroceleTestByAge(j, j + 1, HydroceleTotalWorms,
                                          HydroceleShape)
              << "\n";
    }
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "Lymphodema"
              << ","
              << popln.LymphodemaTestByAge(j, j + 1, LymphodemaTotalWorms,
                                           LymphodemaShape)
              << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << popln.HydroceleTestByAge(j, j + 1, HydroceleTotalWorms,
                                          HydroceleShape)
              << "\n";
    }
    for (int j = 0; j < maxAge; j++) {
      outfile << popln.LymphodemaTestByAge(j, j + 1, LymphodemaTotalWorms,
                                           LymphodemaShape)
              << "\n";
    }
  }

  outfile.close();
}

void Scenario::InitNTDMCData(int rep, std::string folder) {
  // get mf prevalence
  struct stat buffer;

  std::string fname;
  std::string fname2;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  fname = folder + "/NTDMC_scen" + fol_n + "/" + name + "/NTDMC_scen" + name +
          "_rep_" + rep1 + ".csv";
  fname2 = folder + "/NTDMC_scen" + fol_n + "/" + name;
  if (stat(fname2.c_str(), &buffer) != 0) {
    fs::create_directories(fname2);
  }
  std::ofstream outfile;
  outfile.open(fname);

  if (rep == 0) {
    outfile << "espen_loc"
            << ","
            << "year_id"
            << ","
            << "age_start"
            << ","
            << "age_end"
            << ","
            << "measure"
            << ","
            << "draw_0"
            << "\n";
  } else {
    outfile << "draw_0"
            << "\n";
  }
  outfile.close();
}

void Scenario::writeMDADataAllTreated(int t,
                                      const std::vector<int> &numTreatedByAge,
                                      const std::vector<int> &numHostsByAge,
                                      int maxAge, int rep, std::string type,
                                      std::string folder) {

  assert(numHostsByAge.size() == maxAge);
  assert(numTreatedByAge.size() == maxAge);
  std::ofstream outfile;
  std::string fname;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);

  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
          "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Unable to open file " << fname << " for writing."
              << std::endl;
    return; // Or handle the error appropriately
  }

  // we output the number of people who were given doses of drugs, so we can
  // keep track of the distribution across age groups and costs
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "MDA (" << type << ")," << numTreatedByAge[j] << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << numTreatedByAge[j] << "\n";
    }
  }

  // we also want to output the number of people in each age group so that
  // coverages can be calculated in post-processing
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "MDA (" << type << ") number," << numHostsByAge[j] << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << numHostsByAge[j] << "\n";
    }
  }

  outfile.close();
}

void Scenario::writePreTAS(int t, int *numSurvey, int maxAge, int rep,
                           std::string folder) {
  std::ofstream outfile;
  std::string fname;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);

  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/PreTAS_scen" + name +
          "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Unable to open file " << fname << " for writing."
              << std::endl;
    return; // Or handle the error appropriately
  }
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "PreTAS survey ," << numSurvey[j] << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << numSurvey[j] << "\n";
    }
  }

  outfile.close();
}

void Scenario::writeTAS(int t, int *numSurvey, int maxAge, int rep,
                        std::string folder) {
  std::ofstream outfile;
  std::string fname;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);

  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/TAS_scen" + name +
          "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Unable to open file " << fname << " for writing."
              << std::endl;
    return; // Or handle the error appropriately
  }
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << "TAS survey ," << numSurvey[j] << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << numSurvey[j] << "\n";
    }
  }

  outfile.close();
}

void Scenario::writeEmptySurvey(int year, int maxAge, int rep,
                                std::string surveyType, std::string folder) {
  std::ofstream outfile;
  std::string fname;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);

  if (surveyType == "PreTAS survey") {
    fname = folder + "/IHME_scen" + fol_n + "/" + name + "/PreTAS_scen" + name +
            "_rep_" + rep1 + ".csv";
  }
  if (surveyType == "TAS survey") {
    fname = folder + "/IHME_scen" + fol_n + "/" + name + "/TAS_scen" + name +
            "_rep_" + rep1 + ".csv";
  }

  // int year = t/12 + 2000;

  outfile.open(fname, std::ios::app);
  if (!outfile.is_open()) {
    std::cerr << "Error: Unable to open file " << fname << " for writing."
              << std::endl;
    return; // Or handle the error appropriately
  }
  if (rep == 0) {
    for (int j = 0; j < maxAge; j++) {
      outfile << name << "," << year << "," << j << "," << j + 1 << ","
              << surveyType << "," << 0 << "\n";
    }
  } else {
    for (int j = 0; j < maxAge; j++) {
      outfile << 0 << "\n";
    }
  }

  outfile.close();
}

void Scenario::writeSurveyByAge(Population &popln, int t, int preTAS_Pass,
                                int TAS_Pass, int rep, std::string folder) {
  // get mf prevalence
  std::ofstream outfile;

  std::string fname;
  std::string rep1 = std::to_string(rep);
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  fname = folder + "/IHME_scen" + fol_n + "/" + name + "/IHME_scen" + name +
          "_rep_" + rep1 + ".csv";
  int year = t / 12 + 2000;

  outfile.open(fname, std::ios::app);

  if (rep == 0) {
    outfile << name << "," << year << ","
            << "None"
            << ","
            << "None"
            << ","
            << "numPreTASSurveys"
            << "," << popln.numPreTASSurveys << "\n";
    outfile << name << "," << year << ","
            << "None"
            << ","
            << "None"
            << ","
            << "TASSurveys"
            << "," << popln.numTASSurveys << "\n";
    outfile << name << "," << year << ","
            << "None"
            << ","
            << "None"
            << ","
            << "PreTASPass"
            << "," << preTAS_Pass << "\n";
    outfile << name << "," << year << ","
            << "None"
            << ","
            << "None"
            << ","
            << "TASPass"
            << "," << TAS_Pass << "\n";
  } else {
    outfile << popln.numPreTASSurveys << "\n";
    outfile << popln.numTASSurveys << "\n";
    outfile << preTAS_Pass << "\n";
    outfile << TAS_Pass << "\n";
  }

  outfile.close();
}

std::string Scenario::getName() {
  std::size_t first_ = name.find("_");
  std::string fol_n = name.substr(0, first_);
  return fol_n;
}
