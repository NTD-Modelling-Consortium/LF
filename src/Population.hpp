//
//  Population.hpp
//  transfil
//
//  Created by Paul Brown on 03/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Population_hpp
#define Population_hpp

#include "Host.hpp"
#include "RecordedPrevalence.hpp"
#include "Statistics.hpp"
#include "Vector.hpp"
#include <iostream>
#include <map>
#include <random>
#include <string>

// class Vector;
class Worm;
class MDAEvent;
class PrevalenceEvent;
class BedNetEvent;
class TiXmlElement;
class Scenario;

#define MAX_POP 11550

// class to represent a population
// A container class for the Host class and also provides a size distribution
// function

class Population {

  friend std::ostream &operator<<(std::ostream &ostr, const Population &);

public:
  Population(TiXmlElement *xmlParameters);

  ~Population() {
    if (popSize != NULL)
      delete[] popSize;
    if (populationDistribution != NULL)
      delete populationDistribution;
  }

  void loadPopulationSize(const std::string filename);
  static int getMaxAge();
  int getLymphodemaTotalWorms();
  int getHydroceleTotalWorms();
  double getLymphodemaShape();
  double getHydroceleShape();
  double getNeverTreatChangeTime();
  void initHosts(std::string distType, double k_val, double aImp_val);
  double selectPopSizeFromDistribution();
  int PreTASSurvey(Scenario &sc, int forPreTass, int t, int outputEndgameDate,
                   int rep, std::string folderName);
  int TASSurvey(Scenario &sc, int t, int outputEndgameDate, int rep,
                std::string folderName);
  RecordedPrevalence getPrevalence(PrevalenceEvent *outputPrev) const;

  double getLarvalUptakebyVector(double r1, double kappas1,
                                 Vector::vectorSpecies species) const;
  double getBedNetCoverage() const;
  double getBedNetSysComp() const;
  double getImportationRateFactor() const;
  int getSizeOfPop() const;

  double getMFPrev(Scenario &sc, int forPreTass, int t, int outputEndgameDate,
                   int rep, int sampleSize, std::string folderName);
  bool test_for_infection(bool is_infected, float ICsensitivity,
                          float ICspecificity);
  void getIncidence(Scenario &sc, int t, int rep, std::string folderName);
  double getMFPrevByAge(double ageStart, double ageEnd, bool sample);
  double getNumberByAge(double ageStart, double ageEnd);
  double HydroceleTestByAge(int ageStart, int ageEnd, int HydroceleTotalWorms,
                            double HydroceleShape);
  double LymphodemaTestByAge(int ageStart, int ageEnd, int LymphodemaTotalWorms,
                             double LymphodemaShape);
  void initPTreat(double cov, double rho);
  void editPTreat(double cov, double rho);
  void checkForZeroPTreat(double cov, double rho);
  double getICPrev(Scenario &sc, int t, int outputEndgameDate, int rep,
                   std::string folderName);
  double getICPrevForOutput(bool sample);
  void changeNeverTreat();
  void changeICTest();
  void neverTreatToOriginal();
  void ICTestToOriginal();
  void updateKVal(double k_val);
  double getICTestChangeTime();
  void updateBedNetCoverage(BedNetEvent *bn);
  void updateImportationRate(double factor);
  double getNeverTreat();
  void evolve(double dt, const Vector &vectors, const Worm &worms);
  void ApplyTreatment(MDAEvent *mda, Worm &worms, Scenario &sc, int t, int rep,
                      std::string folderName);
  void ApplyTreatmentUpdated(MDAEvent *mda, Worm &worms, Scenario &sc, int t,
                             int roundNumber, int outputEndgameDate, int rep,
                             bool DoMDA, int outputEndgame,
                             std::string folderName);
  void saveCurrentState(int month, std::string sname);
  void resetToMonth(int month);
  void clearSavedMonths();
  double getICSens();

  double getICSpec();
  std::string returnMDAType(MDAEvent *mda);
  int returnMinAgeMDA(MDAEvent *mda);
  std::vector<std::string> printRandomVariableNames() const;
  std::vector<double> printRandomVariableValues() const;
  int returnMaxAge();
  int getSampleSize() const;
  int getMinAgePrev() const;
  int getMinAgeMDA() const;
  int getUpdateParams() const;
  int getNoMDALowMF() const;
  int getSurveyStartDate() const;
  void printMDAHistory() const;
  double ICThreshold;
  double MFThreshold;
  int interSurveyPeriod;
  int firstTASNumMDA;
  // declare the information regarding surveys
  int numPreTASSurveys;
  int numTASSurveys;
  int totMDAs;
  int post2020MDAs;
  int time_TAS_Passes;
  int preTASSurveyTime;
  int TASSurveyTime;
  int preTAS_Pass;
  int TAS_Pass;
  double prevCov;
  double prevRho;
  // indicator if we should do the MDA when the MDA is called.
  // This will be switched to 0 if preTAS is passed, then the MDA function will
  // be called, but will not be done We will still get the output of the MDA
  // showing that no people were treated this year. This is to keep the output
  // of MDA's constant so that we can combine different runs even if they have
  // different numbers of MDA's performed.
  bool DoMDA;
  double aImp;
  int sensSpecChangeCount = 0;
  int neverTreatChangeCount = 0;
  int switchImportationReducingMethodTime =
      99999; // initialise the time to switch importation reduction to a large
             // arbitrary value, so that if we don't specify a different value
             // in the XML scenario file, then it will never be passed
  int removeCoverageReduction =
      0; // set to 0 meaning that if not read from file then we will not remove
         // the reduction to MDA coverage
  int removeCoverageReductionTime =
      99999; // initialise the time to remove coverage reduction to a large
             // arbitrary value, so that if we don't specify a different value
             // in the XML scenario file, then it will never be passed
  int graduallyRemoveCoverageReduction =
      0; // set to 0 meaning that if not read from file then we will not
         // gradually remove the reduction to MDA coverage
  std::vector<std::string> sensSpecChangeName;
  std::vector<std::string> neverTreatChangeName;

private:
  double calcU0(double coverage, double sigma);
  void setU(Host &h, double sigmaMDA, double sigmaBednets);
  void rmvnorm(const int n, const gsl_vector *mean, const gsl_matrix *var,
               gsl_vector *result);

  // param sampling
  std::default_random_engine randgen;
  std::discrete_distribution<int> *populationDistribution;
  int *popSize;

  // The hosts
  Host host_pop[MAX_POP];

  int size;
  // int sizeExtra;

  const static int maxAge = 100;
  double tau; // death rate of host

  double sysCompMDA;
  double sysCompBednets;

  double
      u0CompBednets; // Need to set Host::uCompN for bednet compliance. This is
                     // the mean of a normal distribution from which a random
                     // value is take to determine whether host complies or not
                     // Set to a value that will give a probability of
                     // compliance equal to the specified coverage covN
  double u0CompMDA;
  double u0CompMDA_adult;

  double k; // shape parameter for gamma distrib used to get biteRisk

  double rhoBU; // correlation between non-compliance and bite risk
  double rhoCN; // correlation of non-compliance between bed-nets and MDA
                // (Chemotherapy)

  double TotalBiteRisk; // sum for whole population

  // max values for random parameters
  double mink;
  double maxk;
  // double maxaImp;
  // double meanaImp;

  double aImp_original;
  double aImp_factor;
  double bedNetCov; // the value when syscomp last changed, ie the value used ot
                    // calculate hosts' u values
  double mdaCoverage;

  int updateParams;
  int NoMDALowMF;
  int minAgePrev; // min age to include in prevalance clac

  int minAgeMDA; // min age to receive MDA

  int minAgeMF;                 // min age to be tested for MF
  int maxAgeMF;                 // max age to be tested for MF
  int minAgeIC;                 // min age to be tested for antigen
  int maxAgeIC;                 // max age to be tested for antigen
  double ICsensitivity;         // sensitivity of antigen test
  double ICspecificity;         // sensitivity of antigen test
  double ICsensitivityOriginal; // sensitivity of antigen test
  double ICspecificityOriginal; // sensitivity of antigen test
  double ICsensitivityChange;   // sensitivity of antigen test
  double ICspecificityChange;   // sensitivity of antigen test
  double ICTestChangeTime;

  int HydroceleTotalWorms;
  double HydroceleShape;
  int LymphodemaTotalWorms;
  double LymphodemaShape;
  double neverTreated;
  double neverTreatedOriginal;
  double neverTreatedChange;
  double neverTreatedChangeTime;
  int surveyStartDate;
  int sampleSize = 250; // set a default sample size for pre tas survey. This
                        // can be changed in the xml file
  // via a sampleSize param name in the population parameters

  // saved months

  typedef struct {

    std::vector<hostState> data;
    int month;
    double aImp;
    double sysCompMDA;
    double sysCompBednets;
    double u0bednets;
    double u0MDA;
    double bednetCov;
    double mdaCov;
    bool DoMDA;
    int totMDAs;
    int numPreTASSurveys;
    int numTASSurveys;
    int preTASSurveyTime;
    int TASSurveyTime;
    int preTAS_Pass;
    int TAS_Pass;
    double prevCov;
    double prevRho;
    std::string
        scenario; // for debugging only. The scenario that saved this month

  } savedMonth;

  std::vector<savedMonth> savedMonths;
};

#endif /* Population_hpp */
