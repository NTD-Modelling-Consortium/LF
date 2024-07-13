//
//  Output.hpp
//  transfil
//
//  Created by Paul Brown on 24/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Output_hpp
#define Output_hpp

#include "RecordedPrevalence.hpp"
#include <string>
#include <vector>

class MDAEvent;
class Population;
class PrevalenceEvent;

// a sub class to represent one col in the results file

class monthToOuput {

  friend class Output;

public:
  monthToOuput()
      : month(0), bedNetCov(0.0), bednetSysComp(-1.0), aImpFactor(1.0),
        prevalenceNeeded(false), minAgePrev(0), mdaNeeded(false), mdaCov(0.0),
        mdaType(std::string("-")), mdaSysComp(-1.0), minAgeMDA(0) {};
  monthToOuput(int mnth, double bnc, double bns, double aif)
      : month(mnth), bedNetCov(bnc), bednetSysComp(bns), aImpFactor(aif),
        prevalenceNeeded(false), minAgePrev(0), mdaNeeded(false), mdaCov(0.0),
        mdaType(std::string("-")), mdaSysComp(-1.0), minAgeMDA(0) {};

private:
  int month;

  double bedNetCov;
  double bednetSysComp;
  double aImpFactor;

  bool prevalenceNeeded;
  int minAgePrev;
  RecordedPrevalence prevalence;

  bool mdaNeeded;
  double mdaCov;
  std::string mdaType;
  double mdaSysComp;
  int minAgeMDA;

  void setMDA(MDAEvent *mda, int age);
  void setPrevalence(int age, RecordedPrevalence prev);
};

// class to define the outputs of the model

class Output {

public:
  Output &operator=(const Output &);

  Output() {};
  Output(int byear) : baseYear(byear) {}

  void initialise();
  void resetToMonth(int month);
  int getBaseYear() { return baseYear; };

  void saveMonth(int month, Population &popl, PrevalenceEvent *outputNeeded,
                 RecordedPrevalence prevalence, MDAEvent *mda = NULL);

  std::string printDate(int n) const;
  std::string printYearIndex(int n) const;
  std::string printYear(int n) const;
  std::string printMonthIndex(int n) const;
  double printBedNetCoverage(int n) const;
  std::string printBednetSysComp(int n) const;
  double printAImpFactor(int n) const;
  std::string printMDACoverage(int n) const;
  std::string printMDASysComp(int n) const;
  std::string printMDAType(int n) const;
  RecordedPrevalence *printPrevalence(int n);
  int getSize() const { return int(months.size()); };

  void clearRandomValues();

  void saveRandomNames(std::vector<std::string> names);
  void saveRandomValues(std::vector<double> vals);
  void saveSeedValue(unsigned long int vals);
  int getNumRandomVars();
  std::string &getRandomVarNames(int idx);
  double getRandomVarValues(int idx);
  unsigned long int getSeedValue();

  std::string getMinAgeForTreatment(int n) const;
  std::string getMinAgeForPrevalence(int n) const;

private:
  int baseYear;

  std::vector<std::string> randomVarNames;
  std::vector<double> randomVarValues;
  unsigned long int seedValue;

  std::vector<monthToOuput> months;
  std::string monthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
};

#endif /* Output_hpp */
