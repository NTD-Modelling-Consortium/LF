//
//  Population.cpp
//  transfil
//
//  Created by Paul Brown on 03/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "Population.hpp"
#include "BedNetEvent.hpp"
#include "MDAEvent.hpp"
#include "PrevalenceEvent.hpp"
#include "Scenario.hpp"
#include "Worm.hpp"
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <sstream>

extern bool _DEBUG;

extern Statistics stats;

Population::Population(TiXmlElement *xmlParameters) {

  neverTreatedChangeTime = 10000;
  int gotAll = 0;

  TiXmlElement *xmlHost = xmlParameters->FirstChildElement("host");

  if (xmlHost == NULL) {
    std::cout << "Error in Population::Population. Cannot find host parameters."
              << std::endl;
    exit(1);
  }

  TiXmlElement *xmlParam = NULL;

  for (xmlParam = xmlHost->FirstChildElement("param"); xmlParam != NULL;
       xmlParam = xmlParam->NextSiblingElement("param")) {

    std::string name;
    double value;
    name.assign(xmlParam->Attribute("name"));
    xmlParam->Attribute("value", &value);

    if (name == "tau") {
      tau = value;
      gotAll++;
    } else if (name == "rhoBU") {
      rhoBU = value;
      gotAll++;
    } else if (name == "rhoCN") {
      rhoCN = value;
      gotAll++;
    } else if (name == "minAgePrev") {
      minAgePrev = int(value);
      gotAll++;
    } else if (name == "minAgeMDA") {
      minAgeMDA = int(value);
      gotAll++;
    } else if (name == "minAgeMF") {
      minAgeMF = int(value);
      gotAll++;
    } else if (name == "maxAgeMF") {
      maxAgeMF = int(value);
      gotAll++;
    } else if (name == "minAgeIC") {
      minAgeIC = int(value);
      gotAll++;
    } else if (name == "maxAgeIC") {
      maxAgeIC = int(value);
      gotAll++;
    } else if (name == "surveyStartDate") {
      surveyStartDate = int(value);
      gotAll++;
    } else if (name == "ICThreshold") {
      ICThreshold = value;
      gotAll++;
    } else if (name == "MFThreshold") {
      MFThreshold = value;
      gotAll++;
    } else if (name == "ICsensitivity") {
      ICsensitivity = value;
      gotAll++;
    } else if (name == "ICspecificity") {
      ICspecificity = value;
      gotAll++;
    } else if (name == "ICsensitivityChange") {
      ICsensitivityChange = value;
      gotAll++;
    } else if (name == "ICspecificityChange") {
      ICspecificityChange = value;
      gotAll++;
    } else if (name == "ICTestChangeTime") {
      ICTestChangeTime = value;
      gotAll++;
    } else if (name == "interSurveyPeriod") {
      interSurveyPeriod = value;
      gotAll++;
    } else if (name == "firstTASNumMDA") {
      firstTASNumMDA = value;
      gotAll++;
    } else if (name == "HydroceleTotalWorms") {
      HydroceleTotalWorms = int(value);
      gotAll++;
    } else if (name == "HydroceleShape") {
      HydroceleShape = value;
      gotAll++;
    } else if (name == "LymphodemaTotalWorms") {
      LymphodemaTotalWorms = int(value);
      gotAll++;
    } else if (name == "LymphodemaShape") {
      LymphodemaShape = value;
      gotAll++;
    } else if (name == "neverTreated") {
      neverTreated = value;
      gotAll++;
    } else if (name == "neverTreatedChange") {
      neverTreatedChange = value;
      gotAll++;
    } else if (name == "neverTreatedChangeTime") {
      neverTreatedChangeTime = value;
      gotAll++;
    } else if (name == "updateParams") {
      updateParams = value;
      gotAll++;
    } else if (name == "NoMDALowMF") {
      NoMDALowMF = value;
      gotAll++;
    } else if (name == "sampleSize") {
      sampleSize = value;

    } else if (name == "sensSpecChangeScen") {
      sensSpecChangeName.push_back(std::to_string(int(value)));
      sensSpecChangeCount = sensSpecChangeCount + 1;
    } else if (name == "neverTreatChangeScen") {
      neverTreatChangeName.push_back(std::to_string(int(value)));
      neverTreatChangeCount = neverTreatChangeCount + 1;
    } else
      std::cout << "Unknown parameter " << name << " in Host parameter list."
                << std::endl;
  }

  // for(int i = 0; i < sensSpecChangeCount; i++){
  //     std::cout << sensSpecChangeName[i] << std::endl;
  // }

  if (gotAll < 28) {
    std::cout << "Error in Population::Population. File  does not contain all "
                 "the required parameters."
              << std::endl;
    exit(1);
  } else
    std::cout << "Population parameters: tau=" << tau << ", rhoBU=" << rhoBU
              << ", rhoCN=" << rhoCN << ", minAgePrev=" << minAgePrev
              << ", minAgeMDA=" << minAgeMDA << std::endl
              << std::endl;

  neverTreatedOriginal = neverTreated;
  ICsensitivityOriginal = ICsensitivity;
  ICspecificityOriginal = ICspecificity;
}

double Population::calcU0(double coverage, double sigma2) {

  // u0 is the mean of the distribution of p values from which individual host p
  // values for MDA/bednet adherence are drawn sigma2 is the variance and
  // determines systematic adherence. High variance means high systematic
  // adherence as p vlaues will be widely distributed Coverage determines the
  // absolute values as the mean of all host p values must be equal to this

  return -1 * stats.cdf_normal_Pinv(coverage, 1.0) *
         sqrt(1 + sigma2); // inverse cdf for gaussian distrib with SD=1
}

void Population::loadPopulationSize(const std::string popfile) {

  // load csv file
  std::ifstream infile(popfile, std::ios_base::in);

  if (!infile.is_open()) {
    std::cout << "Error in Population::loadPopulationSize. Cannot read file"
              << popfile << std::endl;
    exit(1);
  }

  // How many lines?
  int numLines = 0;
  std::string line, size, prob;

  while (getline(infile, line))
    numLines++;

  if (numLines > 2) {
    std::cerr << "We currently only support a file with one population size "
                 "entry from file: "
              << popfile << std::endl;
    exit(1);
  }

  // read file
  infile.clear();
  infile.seekg(0, std::ios::beg);

  popSize = new int[--numLines]; //-1 as header line
  std::vector<double> popWeight; // assumption that col2 in file will sum to 1.0
  popWeight.reserve(numLines);

  getline(infile, line); // ignore header
  for (int i = 0; i < numLines; i++) {

    getline(infile, size, ',');
    getline(infile, prob);
    popSize[i] = atoi(size.c_str());
    popWeight.push_back(atof(prob.c_str()));
  }

  infile.close();

  if (*std::max_element(popSize, popSize + numLines) > MAX_POP) {
    std::cout << "Error in Population::loadPopulationSize. The population size "
                 "cannot exceed "
              << MAX_POP << std::endl;
    exit(1);
  }

  // set up discrete sampler
  populationDistribution =
      new std::discrete_distribution<int>(popWeight.begin(), popWeight.end());

  // seed random generator
  randgen.seed(
      (unsigned)std::chrono::system_clock::now().time_since_epoch().count());
}

double Population::getPopSize() {

  // use distribution defined above to get a population size

  unsigned choice = (*populationDistribution)(randgen);
  // this a number in range 0 to numLines-1
  return popSize[choice];
}

void Population::initPTreat(double cov, double rho) {
  if (rho > 0) {
    double alpha = cov * (1 - rho) / rho;
    double beta = (1 - cov) * (1 - rho) / rho;
    for (int i = 0; i < size; i++) {
      host_pop[i].initialisePTreat(alpha, beta);
    }
  } else {
    for (int i = 0; i < size; i++) {
      host_pop[i].pTreat = cov;
    }
  }
}

struct CompareArray {
  const double *myArray;

  CompareArray(const double *arr) : myArray(arr) {}

  bool operator()(int a, int b) const { return myArray[a] < myArray[b]; }
};

void Population::editPTreat(double cov, double rho) {
  if (rho > 0) {
    // define the parameters for the probability of treatment
    double alpha = cov * (1 - rho) / rho;
    double beta = (1 - cov) * (1 - rho) / rho;
    // create array to hold these probabilities
    double pTreats[size];
    // draw probabilities from the beta distribution
    for (int i = 0; i < size; i++) {
      pTreats[i] = stats.beta_dist(alpha, beta);
    }
    // sort these values so that they are in ascending order
    std::sort(pTreats, pTreats + size);

    // We want to know the rank of each of the host populations pTreat so that
    // we can maintain the order of their probability of treatment under new
    // coverage and rho values by assigning the newly drawn probabilities to the
    // appropriate individual

    // Create an array of indices
    int indices[size];

    for (int i = 0; i < size; i++) {
      indices[i] = i;
    }
    // store the current value of the pTreat in an array
    double oldPTreat[size];
    // std::cout << "OLDPtreat" << std::endl;
    for (int i = 0; i < size; i++) {
      oldPTreat[i] = host_pop[i].pTreat;
    }

    // Sort the indices array based on the values in oldPTreat
    std::sort(indices, indices + size, CompareArray(oldPTreat));

    // now assign the value from the newly drawn beta distribution stored in
    // pTreats to the appropriate place in the host_pop based on the rank of the
    // current values there which are stored in the oldPTreat array. This works
    // as the entries in the pTreats array are already in ascending order.
    // std::cout << "NEWPtreat" << std::endl;
    // Assign the newly drawn treatment probabilities to the appropriate
    // individuals
    for (int i = 0; i < size; i++) {
      host_pop[indices[i]].pTreat = pTreats[i];
    }
  } else {
    for (int i = 0; i < size; i++) {
      host_pop[i].pTreat = cov;
    }
  }
}

void Population::checkForZeroPTreat(double cov, double rho) {

  // define the parameters for the probability of treatment
  if (rho > 0) {
    double alpha = cov * (1 - rho) / rho;
    double beta = (1 - cov) * (1 - rho) / rho;
    for (int i = 0; i < size; i++) {
      if (host_pop[i].pTreat == 0) {
        host_pop[i].pTreat = stats.beta_dist(alpha, beta);
      }
    }
  } else {
    for (int i = 0; i < size; i++) {
      if (host_pop[i].pTreat == 0) {
        host_pop[i].pTreat = cov;
      }
    }
  }

  // loop over pTreat values and if there are any zero values, then
  // draw from beta distribution to set the pTreat value.
}

int Population::getUpdateParams() const { return updateParams; }

int Population::getNoMDALowMF() const { return NoMDALowMF; }

int Population::getSurveyStartDate() const { return surveyStartDate; }

void Population::initHosts(std::string distType, double k_val,
                           double aImp_val) {

  // generate a new population size and reset all members at the start of a new
  // replicate
  size = getPopSize();
  TotalBiteRisk = 0.0;

  // new random value for k, shape of gamma distrib
  k = mink + (stats.uniform_dist() *
              (maxk - mink)); // must be in range 0.01 <= k <= 0.14

  // //new random value for imporation rate
  // if(distType == "uniform")
  //     aImp_original = (stats.uniform_dist() * maxaImp); //must be in range 0
  //     <= aImp <= 0.002
  // else
  //     while((aImp_original = stats.exp_dist(meanaImp)) > maxaImp);

  // tmp fix here
  k = k_val;
  aImp_original = aImp_val;

  aImp = aImp_original; // save this as it need to appear in output file

  for (int i = 0; i < size;
       i++) // TotalBiteRisk sums bites per month for whole popln
    host_pop[i].initialise(
        tau, maxAge, k, &TotalBiteRisk, HydroceleShape, LymphodemaShape,
        neverTreated); // sets worm count to 0 and treated/bednet to 0. Sets
                       // random age and bite risk

  u0CompBednets =
      std::numeric_limits<double>::max(); // initial value indicates first time
                                          // bednets applied in this replicate
  u0CompMDA = std::numeric_limits<double>::max(); // define this the first time
                                                  // mda applied

  sysCompMDA = 0.0;
  sysCompBednets = 0.99;

  bedNetCov = 0.0;
  aImp_factor = 1.0;
  mdaCoverage = 0.0;
}

void Population::updateKVal(double k_val) {
  k = k_val;
  TotalBiteRisk = 0.0;
  // create array to hold these probabilities
  double biteRisk[size];
  // draw probabilities from the beta distribution
  for (int i = 0; i < size; i++) {
    biteRisk[i] = stats.gamma_dist(k);
    TotalBiteRisk += biteRisk[i];
  }
  // sort these values so that they are in ascending order
  std::sort(biteRisk, biteRisk + size);
  // We want to know the rank of each of the host populations pTreat so that we
  // can maintain the order of their probability of treatment under new coverage
  // and rho values by assigning the newly drawn probabilities to the
  // appropriate individual

  // Create an array of indices
  int indices[size];

  for (int i = 0; i < size; i++) {
    indices[i] = i;
  }
  // store the current value of the pTreat in an array
  double oldbiteRisk[size];
  for (int i = 0; i < size; i++) {
    oldbiteRisk[i] = host_pop[i].biteRisk;
  }

  // Sort the indices array based on the values in oldPTreat
  std::sort(indices, indices + size, CompareArray(oldbiteRisk));

  // now assign the value from the newly drawn beta distribution stored in
  // pTreats to the appropriate place in the host_pop based on the rank of the
  // current values there which are stored in the oldPTreat array. This works as
  // the entries in the pTreats array are already in ascending order.

  // Assign the newly drawn treatment probabilities to the appropriate
  // individuals
  for (int i = 0; i < size; i++) {
    host_pop[indices[i]].biteRisk = biteRisk[i];
  }
}

RecordedPrevalence
Population::getPrevalence(PrevalenceEvent *outputPrev) const {

  int numHosts = 0;
  int numHostsExtra = 0;
  int minAgeinMonths;
  int minAgeInMonthsExtra;
  int maxAgeInMonthsExtra;
  bool requiresExtra = false;
  RecordedPrevalence prevalence(size); // initi for poln size so can save ages

  bool needsMF = outputPrev->getMethod("mf");
  bool needsIC = outputPrev->getMethod("ic");
  bool needsWC = outputPrev->getMethod("wc"); // This is mean worms per person

  if (outputPrev->getMinAge() < 0)
    minAgeinMonths =
        minAgePrev *
        12; // default defined at popln level if not overridden by ouput
  else
    minAgeinMonths = outputPrev->getMinAge() * 12;

  minAgeInMonthsExtra = outputPrev->getMinAgeExtra() * 12;
  maxAgeInMonthsExtra = outputPrev->getMaxAgeExtra() * 12;

  if ((minAgeInMonthsExtra > 0 || maxAgeInMonthsExtra < (maxAge * 12))) {
    requiresExtra = true; // if either set
  }

  for (int i = 0; i < size; i++) {

    bool infectedMF =
        needsMF &&
        (stats.uniform_dist() <
         (1 - exp(-1 * host_pop[i].M))); // depends on how many mf present
    bool infectedIC = needsIC && (host_pop[i].WF > 0 ||
                                  host_pop[i].WM > 0); // at least 1 adult worm

    if (host_pop[i].age >= minAgeinMonths) {
      numHosts++;
      if (infectedMF)
        prevalence.MF++;
      if (infectedIC)
        prevalence.IC++;
      if (needsWC)
        prevalence.WC += (host_pop[i].WF + host_pop[i].WM);
    }

    if (requiresExtra) {

      if ((host_pop[i].age >= minAgeInMonthsExtra) &&
          (host_pop[i].age <= maxAgeInMonthsExtra)) {
        numHostsExtra++;
        if (infectedMF)
          prevalence.MFRestrictedAge++;
        if (infectedIC)
          prevalence.ICRestrictedAge++;
        if (needsWC)
          prevalence.WCRestrictedAge += (host_pop[i].WF + host_pop[i].WM);
      }
    }

    prevalence.saveAge(host_pop[i].age);
  }

  if (numHosts) {
    prevalence.MF /= numHosts;
    prevalence.IC /= numHosts;
    prevalence.WC /= numHosts;
  }
  if (numHostsExtra) {
    prevalence.MFRestrictedAge /= numHostsExtra;
    prevalence.ICRestrictedAge /= numHostsExtra;
    prevalence.WCRestrictedAge /= numHostsExtra;
  }

  return prevalence;
}

int Population::PreTASSurvey(Scenario &sc, int forPreTass, int t,
                             int outputEndgameDate, int rep,
                             std::string folderName) {
  int preTAS_Pass = 0;
  // get the mfprevalence via a survey with sample size specified by sampleSize
  // variable this is set to 250 by default, but can be changed via inputting a
  // variable in the xml file
  double mfprev = getMFPrev(sc, forPreTass, t, outputEndgameDate, rep,
                            sampleSize, folderName); // find the mf prevalence
  numPreTASSurveys += 1;       // increment number of pre-TAS tests by 1
  if (mfprev <= MFThreshold) { // if the mf prevalence is below threshold
    preTAS_Pass = 1;           // set pre-TAS pass indicator to 1
  }
  return preTAS_Pass;
}

int Population::TASSurvey(Scenario &sc, int t, int outputEndgameDate, int rep,
                          std::string folderName) {
  int TAS_Pass = 0;

  double icprev =
      getICPrev(sc, t, outputEndgameDate, rep,
                folderName);     // find ic prevalence in specified age group
  numTASSurveys += 1;            // increment number of TAS surveys by 1
  if ((icprev <= ICThreshold)) { // if the ic prevalence is below the threshold
                                 // and mf prev also below threshold
    TAS_Pass = 1;                // set TAS pass indicator to 1
    t_TAS_Pass = t;              // store the time of passing TAS
  }
  return TAS_Pass;
}

double Population::getMFPrev(Scenario &sc, int forPreTass, int t,
                             int outputEndgameDate, int rep, int sampleSize,
                             std::string folderName) {
  // get mf prevalence
  // we also store the number of people who are surveyed in each age group
  // as this may be output for IHME to use

  double MFpos = 0;           // number of people mf positive
  double numHostsSampled = 0; // total number of hosts sampled so far
  int maxAgeMonths = maxAgeMF * 12;
  int minAgeMonths = minAgeMF * 12;

  int numSurvey[maxAge];
  for (int i = 0; i < maxAge; ++i) {
    numSurvey[i] = 0; // initialization
  }
  std::vector<int> people_indices(size);
  std::iota(people_indices.begin(), people_indices.end(),
            0); // fill indices with 0, 1, ..., size-1
  stats.shuffle_indices(people_indices);

  for (int person_index : people_indices) {
    // Stop when we have surveyed the number of people specified by sampleSize,
    // as this is the number of people we want to sample
    if (numHostsSampled >= sampleSize)
      break;

    if ((host_pop[person_index].age >= minAgeMonths) &&
        (host_pop[person_index].age <= maxAgeMonths)) {
      // we want to track the number of people of each age who are surveyed so
      // that we can output this later if this is done for a pre TAS survey
      float flooredAge = std::floor(host_pop[person_index].age / 12);
      int flooredAgeInt = std::min(static_cast<int>(flooredAge), maxAge - 1);
      numSurvey[flooredAgeInt] += 1;

      bool infectedMF =
          (stats.uniform_dist() <
           (1 - exp(-1 * host_pop[person_index]
                             .M))); // probability of being measured as infected
                                    // depends on how many mf present
      numHostsSampled++;            // increment number of hosts sampled by 1
      if (infectedMF)
        MFpos++; // if mf positive, increment MFpos by 1
    }
  }

  // If we are getting the mf prevalence for the preTAS survey then we want to
  // ouptut this to the endgame outputs. However if we are before the date of
  // the earliest date of outputs for endgame then we don't want the output as
  // we may do this survey at an earlier date than we are interested outputting.
  if ((forPreTass == 1) && (t >= outputEndgameDate)) {
    sc.writePreTAS(t, numSurvey, maxAge, rep, folderName);
  }

  if (numHostsSampled > 0) {
    return MFpos /
           numHostsSampled; // convert to prevalence of mf positive hosts
  } else {
    return 0.0;
  }
}

void Population::getIncidence(Scenario &sc, int t, int rep,
                              std::string folderName) {
  // get incidence

  int incidence[maxAge];
  for (int i = 0; i < maxAge; ++i) {
    incidence[i] = 0; // initialization
  }

  for (int i = 0; i < size; i++) {
    float flooredAge = std::floor(host_pop[i].age / 12);
    int flooredAgeInt = std::min(static_cast<int>(flooredAge), maxAge - 1);
    bool infectedMF =
        (stats.uniform_dist() <
         (1 - exp(-1 * host_pop[i].M))); // depends on how many mf present

    if (infectedMF) {

      if (host_pop[i].previouslyInfected == 0) {
        incidence[flooredAgeInt] += 1; // if mf positive, increment MFpos by 1
        host_pop[i].previouslyInfected = 1;
      }
    } else {
      host_pop[i].previouslyInfected = 0;
    }
  }
  sc.writeIncidence(t, incidence, maxAge, rep, folderName);
}

double Population::getMFPrevByAge(double ageStart, double ageEnd, bool sample) {
  // get mf prevalence

  double MFpos = 0;           // number of people mf positive
  double numHostsSampled = 0; // total number of hosts
  int minAgeMonths = ageStart * 12;
  int maxAgeMonths = ageEnd * 12;
  bool infectedMF;
  for (int i = 0; i < size; i++) {
    if ((host_pop[i].age >= minAgeMonths) && (host_pop[i].age < maxAgeMonths)) {
      if (sample) {
        infectedMF =
            (stats.uniform_dist() <
             (1 - exp(-1 * host_pop[i].M))); // depends on how many mf present
      } else {
        infectedMF = host_pop[i].M > 0; // as true prev, we just report if there
                                        // are any mf present at all
      }
      numHostsSampled++; // increment number of hosts by 1
      if (infectedMF)
        MFpos++; // if mf positive, increment MFpos by 1
    }
  }
  if (numHostsSampled > 0) {
    return MFpos /
           numHostsSampled; // convert to prevalence of mf positive hosts
  } else {
    return 0.0;
  }
}

double Population::getNumberByAge(double ageStart, double ageEnd) {
  // get mf prevalence

  double numHosts = 0; // total number of hosts
  int minAgeMonths = ageStart * 12;
  int maxAgeMonths = ageEnd * 12;
  for (int i = 0; i < size; i++) {
    if ((host_pop[i].age >= minAgeMonths) && (host_pop[i].age < maxAgeMonths)) {
      numHosts++; // increment number of hosts by 1
    }
  }

  return numHosts;
}

double Population::HydroceleTestByAge(int ageStart, int ageEnd,
                                      int HydroceleTotalWorms,
                                      double HydroceleShape) {
  // get proportion of people in a given age bracket who are hydrocele positive

  double HydroPos = 0;        // number of people mf positive
  double numHostsSampled = 0; // total number of hosts
  int minAgeMonths = ageStart * 12;
  int maxAgeMonths = ageEnd * 12;
  double mult = 0;
  for (int i = 0; i < size; i++) {

    if ((host_pop[i].age >= minAgeMonths) &&
        (host_pop[i].age <= maxAgeMonths) &&
        (host_pop[i].sex == 0)) { // only men can get hydrocele
      mult =
          host_pop[i].hydroMult; // draw random number from gamma distribution
                                 // with appropriate shape which gives
                                 // individual susceptibility to sequelae
      bool Hydro = ((mult * host_pop[i].totalWorms) >
                    HydroceleTotalWorms); // individual susceptibility * total
                                          // worms > threshold or not
      numHostsSampled++;                  // increment number of hosts by 1
      if (Hydro)
        HydroPos++; // if hydrocele positive, increment hydrocelePos by 1
    }
  }
  if (numHostsSampled > 0) {
    return HydroPos /
           numHostsSampled; // convert to prevalence of hydrocele positive hosts
  } else {
    return 0.0;
  }
}

double Population::LymphodemaTestByAge(int ageStart, int ageEnd,
                                       int LymphodemaTotalWorms,
                                       double LymphodemaShape) {
  // get proportion of people in a given age bracket who are lymphodema positive

  double LymphodemaPos = 0;   // number of people mf positive
  double numHostsSampled = 0; // total number of hosts
  int minAgeMonths = ageStart * 12;
  int maxAgeMonths = ageEnd * 12;
  double mult = 0;
  for (int i = 0; i < size; i++) {

    if ((host_pop[i].age >= minAgeMonths) &&
        (host_pop[i].age <= maxAgeMonths)) { // only men can get hydrocele
      mult =
          host_pop[i].lymphoMult; // draw random number from gamma distribution
                                  // with appropriate shape which gives
                                  // individual susceptibility to sequelae
      bool Lymphodema =
          ((mult * host_pop[i].totalWorms) >
           LymphodemaTotalWorms); // individual susceptibility * total worms >
                                  // threshold or not
      numHostsSampled++;          // increment number of hosts by 1
      if (Lymphodema)
        LymphodemaPos++; // if lymphodema positive, increment LymphodemaPos by 1
    }
  }
  if (numHostsSampled > 0) {
    return LymphodemaPos /
           numHostsSampled; // convert to prevalence of hydrocele positive hosts
  } else {
    return 0.0;
  }
}

bool Population::test_for_infection(bool is_infected, float ICsensitivity,
                                    float ICspecificity) {
  if (is_infected) {
    return stats.uniform_dist() < ICsensitivity;
  } else {
    return stats.uniform_dist() < 1 - ICspecificity;
  }
}

double Population::getICPrev(Scenario &sc, int t, int outputEndgameDate,
                             int rep, std::string folderName) {
  // get IC prevalence. This is modelled by sensing the presence of any adult
  // worms we also store the number of people who are surveyed in each age group
  // as this may be output for IHME to use

  double ICpos = 0;           // number of people ic positive
  double numHostsSampled = 0; // total number of hosts
  int maxAgeMonths = maxAgeIC * 12;
  int minAgeMonths = minAgeIC * 12;
  int numSurvey[maxAge];
  for (int i = 0; i < maxAge; ++i) {
    numSurvey[i] = 0;
  }
  for (int i = 0; i < size; i++) {
    if ((host_pop[i].age < maxAgeMonths) && (host_pop[i].age >= minAgeMonths)) {
      bool is_infected = (host_pop[i].WF + host_pop[i].WM) > 0;
      float flooredAge = std::floor(host_pop[i].age / 12);
      int flooredAgeInt = std::min(static_cast<int>(flooredAge), maxAge - 1);
      numSurvey[flooredAgeInt] += 1;
      bool infectedIC =
          test_for_infection(is_infected, ICsensitivity, ICspecificity);
      numHostsSampled++; // increment number of hosts by 1
      if (infectedIC)
        ICpos++; // if IC positive, increment ICpos by 1
    }
  }
  // If we are getting the IC prevalence for the TAS survey then we want to
  // ouptut this to the endgame outputs. However if we are before the date of
  // the earliest date of outputs for endgame then we don't want the output as
  // we may do this survey at an earlier date than we are interested outputting.
  if (t >= outputEndgameDate) {
    sc.writeTAS(t, numSurvey, maxAge, rep, folderName);
  }
  if (numHostsSampled > 0) {
    return ICpos / numHostsSampled; // convert to prevalence rather than number
                                    // of infected
  } else {
    return 0.0;
  }
}

double Population::getICPrevForOutput(bool sample) {
  // get IC prevalence. This is modelled by sensing the presence of any adult
  // worms sample is true if we want to take a sample of the population by
  // including specificity and sensitivity of the test sample is false if we
  // want to give the actual prevalence of the population

  double ICpos = 0;           // number of people ic positive
  double numHostsSampled = 0; // total number of hosts
  int maxAgeMonths = maxAgeIC * 12;
  int minAgeMonths = minAgeIC * 12;
  bool infectedIC;

  for (int i = 0; i < size; i++) {
    if ((host_pop[i].age < maxAgeMonths) && (host_pop[i].age >= minAgeMonths)) {
      bool is_infected = (host_pop[i].WF + host_pop[i].WM) > 0;

      if (sample) {
        infectedIC =
            test_for_infection(is_infected, ICsensitivity, ICspecificity);
      } else {
        infectedIC = is_infected;
      }
      numHostsSampled++; // increment number of hosts by 1
      if (infectedIC)
        ICpos++; // if mf positive, increment MFpos by 1
    }
  }
  if (numHostsSampled > 0) {
    return ICpos / numHostsSampled; // convert to prevalence rather than number
                                    // of infected
  } else {
    return 0.0;
  }
}

double
Population::getLarvalUptakebyVector(double r1, double kappas1,
                                    Vector::vectorSpecies species) const {

  // this is the rate at which the vector population takes up l3 larvae from
  // each host return total uptake for whole population weighted according to
  // each hosts's bite risk

  double mf = 0.0;
  double uptake;
  double TotalBiteRisk = 0;
  for (int i = 0; i < size; i++) {

    uptake = (1 - exp(-r1 * host_pop[i].M / kappas1));
    if (species == Vector::Anopheles)
      uptake *= uptake;
    mf += host_pop[i].biteRisk * kappas1 * uptake;
    TotalBiteRisk += host_pop[i].biteRisk;
  }
  return mf / TotalBiteRisk;
}

void Population::evolve(double dt, const Vector &vectors, const Worm &worms) {

  // advance one time step
  for (int i = 0; i < size; i++) {
    host_pop[i].react(dt, tau, maxAge, aImp, vectors, worms, HydroceleShape,
                      LymphodemaShape, neverTreated);
  }
}

int Population::getSampleSize() const { return sampleSize; }

int Population::getMaxAge() { return maxAge; }

int Population::getLymphodemaTotalWorms() { return LymphodemaTotalWorms; }

int Population::getHydroceleTotalWorms() { return HydroceleTotalWorms; }

double Population::getLymphodemaShape() { return LymphodemaShape; }

double Population::getHydroceleShape() { return HydroceleShape; }

double Population::getNeverTreatChangeTime() { return neverTreatedChangeTime; }

double Population::getNeverTreat() { return neverTreated; }

void Population::changeNeverTreat() {
  neverTreated = neverTreatedChange;
  for (int i = 0; i < size; i++) {
    if (host_pop[i].neverTreat == 1) {
      if (stats.uniform_dist() > (neverTreatedChange / neverTreatedOriginal)) {
        host_pop[i].neverTreat = 0;
      }
    }
  }
}

void Population::neverTreatToOriginal() { neverTreated = neverTreatedOriginal; }

double Population::getICTestChangeTime() { return ICTestChangeTime; }

double Population::getICSens() { return ICsensitivity; }
double Population::getICSpec() { return ICspecificity; }
void Population::changeICTest() {
  ICsensitivity = ICsensitivityChange;
  ICspecificity = ICspecificityChange;
}

void Population::ICTestToOriginal() {
  ICsensitivity = ICsensitivityOriginal;
  ICspecificity = ICspecificityOriginal;
}
void Population::saveCurrentState(int month, std::string sname) {

  // save host states and importation rate

  savedMonth currentState;

  currentState.scenario = sname; // debugging only
  currentState.data.resize(size, {0, 0, 0, 0.0, 0.0, 0.0, 0,
                                  0}); // WM, WF,totalWorms, totalWormYears,
                                       // M,biteRisk,age,monthSinceTreated

  for (int i = 0; i < size; i++)
    currentState.data[i] =
        host_pop[i]; // overloaded operator extracts data members to struct

  currentState.month = month;

  currentState.aImp = aImp;
  currentState.sysCompMDA = sysCompMDA;
  currentState.sysCompBednets = sysCompBednets;
  currentState.u0MDA = u0CompMDA;
  currentState.u0bednets = u0CompBednets;
  currentState.mdaCov = mdaCoverage;
  currentState.bednetCov = bedNetCov;

  savedMonths.push_back(currentState);
}

void Population::resetToMonth(int month) {

  // Will always discard any months coming after the one required

  while (savedMonths.size()) {

    savedMonth lastMonth = savedMonths.back();

    if (lastMonth.month == month) {

      // restore host state and importation rate
      for (int i = 0; i < size; i++)
        host_pop[i].restore(lastMonth.data[i]);

      aImp = lastMonth.aImp;
      sysCompMDA = lastMonth.sysCompMDA;
      sysCompBednets = lastMonth.sysCompBednets;
      u0CompMDA = lastMonth.u0MDA;
      u0CompBednets = lastMonth.u0bednets;
      mdaCoverage = lastMonth.mdaCov;
      bedNetCov = lastMonth.bednetCov;

      if (_DEBUG)
        std::cout << "Reset to month " << month << ", that was saved by "
                  << lastMonth.scenario << std::endl;

      return;
    }
    savedMonths.pop_back();
  }

  std::cout
      << "Error in Population::resetToMonth. Cannot find the specified month "
      << month << std::endl;
  exit(1);
}

void Population::updateBedNetCoverage(BedNetEvent *bn) {

  // called by scenario at every time step, before evolve()

  // which individuals have a bednet affects host::react()
  // proportion covered affects Vector::larvalDensity

  // if sysComp has a valid value [0...1] then systematic adherence has changed
  // so need to recaculate u0CompBednets

  // This function affects host.uCompMDA via call to setU!!!! if sigmaMDA > 0,
  // different random noise applied

  bool firstTime = (u0CompBednets == std::numeric_limits<double>::max());
  bool sysCompChanged = (sysCompBednets != bn->getSysComp());
  bool coverageChanged =
      (bedNetCov != bn->getCoverage()); // changed since u0 was calculated so
                                        // need to use scaling factor

  // save host.ucompbednets?? not needed id population u0 saved?

  if (!bn->getCoverage()) { // cov can't be zero or u0CompBednets is infinite

    for (int i = 0; i < size; i++)
      host_pop[i].bedNet = 0;

  } else {

    double coverageScaleFactor = 0.0;

    if (firstTime || sysCompChanged) {

      // first time bednets applied,  so need to generate u0 for population
      // or systematic adherence value has changed so we must re-calculate u0

      u0CompBednets = calcU0(bn->getCoverage(), bn->getSigma2());
      bedNetCov = bn->getCoverage(); // save cov used to calculate u0

      // now generate individual host U values

      if (!bn->getSysComp()) { // sysCompBednets = 0, so all hosts have an equal
                               // probability so no need to draw from normal
                               // distribution
        for (int i = 0; i < size; i++)
          host_pop[i].uCompBednets = u0CompBednets;
      } else if (sysCompMDA <=
                 0) { // sysCompBednets non-zero so simple distribution,
                      // sysComMDA is zero or not set yet.
        for (int i = 0; i < size; i++)
          host_pop[i].uCompBednets = stats.normal_dist(
              u0CompBednets, sqrt(bn->getSigma2())); // same fro norm dis of
                                                     // mean u0 and SD sigma
      } else {
        // systemtic compliance of both, there is a correlation.
        double sigmaMDA = sqrt(sysCompMDA / (1 - sysCompMDA));
        double sigmaBednets = sqrt(bn->getSigma2());

        for (int i = 0; i < size; i++) // alters MDA compliance too. Maybe don't
                                       // do this if not in an mda period
          setU(host_pop[i], sigmaMDA,
               sigmaBednets); // crashes if either input is zero. Sets
                              // uCompBednets and uCompMDA
      }

    } else if (coverageChanged) {

      // systematic adherence is not changing so just need to scale original u0
      // value for new coverage Just scaling it means the same hosts have high
      // and the same have low u values, though absolute values

      coverageScaleFactor =
          calcU0(bn->getCoverage(), bn->getSigma2()) - u0CompBednets;
    }

    // apply new settings

    for (int i = 0; i < size; i++)
      host_pop[i].bedNet =
          (stats.normal_dist(host_pop[i].uCompBednets + coverageScaleFactor,
                             1.0) < 0)
              ? 1
              : 0;
  }

  sysCompBednets = bn->getSysComp();

  if (_DEBUG) {
    int nets = 0;
    for (int i = 0; i < size; i++)
      if (host_pop[i].bedNet)
        nets++;
    std::cout << "specified coverage = " << bn->getCoverage()
              << ", actual coverage = " << double(nets) / double(size)
              << std::endl;
  }
}

void Population::ApplyTreatment(MDAEvent *mda, Worm &worms, Scenario &sc, int t,
                                int rep, std::string folderName) {

  // also must check if syscomp has changed since last scenario!!
  // coverage could change too so meed scalefactor

  // bug with sysCompBednets =0 and L3 doesnt seem to matter

  bool firstTime = (u0CompMDA == std::numeric_limits<double>::max());
  bool sysCompChanged = (sysCompMDA != mda->getCompliance());
  int startTime = sc.getStartMonth();

  bool coverageChanged = (mdaCoverage != mda->getCoverage());

  u0CompMDA = calcU0(mda->getCoverage(), mda->getSigma2());
  for (int i = 0; i < size; i++)
    host_pop[i].uCompMDA = stats.normal_dist(u0CompMDA, sqrt(mda->getSigma2()));
  if (mda->getCoverage()) { // crashes if this is zero, but never should be

    double coverageScaleFactor = 0.0;

    if (firstTime || sysCompChanged || coverageChanged || (t == startTime)) {

      // sysComp has changed so calculate hosts compliance probabilities as a
      // function of coverage and the new syscomp

      // need to create distribution from which hosts p values will be drawn.
      // THis will have mean u0, which depends on coverage and a variance which
      // depends on sysComp. High variance means highly systematic compliance

      u0CompMDA = calcU0(mda->getCoverage(), mda->getSigma2());
      mdaCoverage =
          mda->getCoverage(); // store coverage that was used to calculate this
      if (!mda->getCompliance()) { // syscomp is zero. This means each indivual
                                   // has an equal chance of complying each
                                   // round.
        for (int i = 0; i < size; i++)
          host_pop[i].uCompMDA = u0CompMDA; // MDA value fixed for all hosts, ie
                                            // no systematic adherence

      } else if (sysCompBednets <= 0) {
        // prevents crash if sysCompBednets = 0 AND sysCompMDA != 0
        for (int i = 0; i < size; i++)
          host_pop[i].uCompMDA =
              stats.normal_dist(u0CompMDA, sqrt(mda->getSigma2()));

      } else {

        double sigmaMDA = sqrt(mda->getSigma2());
        double sigmaBednets =
            sqrt(sysCompBednets /
                 (1 - sysCompBednets)); // if this is zero, setU will crash

        for (int i = 0; i < size; i++) {
          setU(
              host_pop[i], sigmaMDA,
              sigmaBednets); // uses sigmaMDA and u0CompMDA to set Host.uCompMDA
          // The call to setU also affects bednet coverage.
          // It will have changed hosts uCompBednets value. This will affect bed
          // net use next month if bed net syscomp has not chnaged
        }
      }

    } else if (coverageChanged || (t == startTime)) {

      // syscomp has not changed but coverage has so scale hosts compliance
      // probabilities as a function of new coverage and the existing syscomp we
      // could recalculate u0CompMDA, but we dont want to do this as we want
      // same host to have or low probabilitie as before

      // coverage has changed since the last time u0CompMDA was set. This would
      // have been set when syscomp changed

      coverageScaleFactor =
          calcU0(mda->getCoverage(), mda->getSigma2()) - u0CompMDA;
    }

    // apply
    int minAge = (mda->getMinAge() >= 0) ? mda->getMinAge() : minAgeMDA;
    int minAgeMDAinMonths = minAge * 12;

    if (_DEBUG)
      std::cout << "Coverage = " << mda->getCoverage() << ", Actual coverage=";

    int hostsOldEnough = 0;
    int hostsTreated = 0;

    for (int i = 0; i < size; i++) {

      if (host_pop[i].age >= minAgeMDAinMonths) {
        hostsOldEnough++;
        if (stats.normal_dist(host_pop[i].uCompMDA + coverageScaleFactor, 1.0) <
            0) {
          if (host_pop[i].neverTreat == 0) {

            host_pop[i].getsTreated(worms, mda->getType());
            hostsTreated++;
          }
        }
      }
    }

    std::string type = mda->getType();
    //   std::cout << "Coverage = " << mda->getCoverage() << ", Actual
    //   coverage=" <<  double(hostsTreated)/hostsOldEnough <<std::endl ;

    if (_DEBUG)
      std::cout << hostsTreated << "/" << hostsOldEnough << " "
                << double(hostsTreated) / hostsOldEnough * 100 << "%"
                << std::endl;
  }

  sysCompMDA = mda->getCompliance();
}

std::string Population::returnMDAType(MDAEvent *mda) {
  std::string MDAType = mda->getType();
  return MDAType;
}

int Population::returnMinAgeMDA(MDAEvent *mda) {
  int minAge = (mda->getMinAge() >= 0) ? mda->getMinAge() : minAgeMDA;
  return minAge;
}

int Population::returnMaxAge() { return maxAge; }

void Population::ApplyTreatmentUpdated(MDAEvent *mda, Worm &worms, Scenario &sc,
                                       int t, int outputEndgameDate, int rep,
                                       int DoMDA, int outputEndgame,
                                       std::string folderName) {
  int minAge = (mda->getMinAge() >= 0) ? mda->getMinAge() : minAgeMDA;
  int minAgeMDAinMonths = minAge * 12;

  if (maxAge <= 0 || size <= 0) {
    std::cerr << "Invalid maxAge or size" << std::endl;
    return;
  }

  int numTreat[maxAge];
  int numHosts[maxAge];
  for (int i = 0; i < maxAge; ++i) {
    numTreat[i] = 0;
    numHosts[i] = 0;
  }

  std::string MDAtype = mda->getType();

  

  for (int i = 0; i < size; i++) {
    float flooredAge = std::floor(host_pop[i].age / 12);
    int flooredAgeInt = std::min(static_cast<int>(flooredAge), maxAge - 1);
    numHosts[flooredAgeInt] += 1;
    if (DoMDA == 1) {
      if (host_pop[i].age >= minAgeMDAinMonths) {
        if (stats.uniform_dist() < host_pop[i].pTreat) {
          if (host_pop[i].neverTreat == 0) {
            host_pop[i].getsTreated(worms, MDAtype);
            numTreat[flooredAgeInt] += 1;
          }
        }
      }
    }
  }
  if ((outputEndgame == 1) && (t >= outputEndgameDate))
    sc.writeMDADataAllTreated(t, numTreat, numHosts, maxAge, rep, MDAtype, folderName);
}

double Population::getBedNetCoverage() const {

  // cant just return bedNetCov, as this might not be up to date
  // its only the vlaue used when syscomp last changed and u values generated.
  // Instead, calculate actual proportion of hosts

  int nets = 0;
  for (int i = 0; i < size; i++)
    if (host_pop[i].bedNet)
      nets++;

  return double(nets) / double(size);
}

double Population::getBedNetSysComp() const { return sysCompBednets; }

double Population::getImportationRateFactor() const { return aImp_factor; }

void Population::updateImportationRate(double factor) {

  aImp *= factor;
  aImp_factor = factor; // just saved for output file
}

void Population::clearSavedMonths() { savedMonths.clear(); }

// print random variables

std::ostream &operator<<(std::ostream &ostr, const Population &pop) {

  return ostr << pop.size << "\t" << pop.k << "\t" << pop.aImp_original;
}

// These 2 functio nreturn that names and values to be printed to file in the
// order they are listed. Must correspond with each other!

std::vector<std::string> Population::printRandomVariableNames() const {

  // outputs a list of name/value pairs of any length. These are passed to
  // output object to be printed to file

  std::vector<std::string> names = {"Population", "k", "aImp"};
  return names;
}

std::vector<double> Population::printRandomVariableValues() const {

  // outputs a list of name/value pairs of any length. These are passed to
  // output object to be printed to file

  std::vector<double> values = {static_cast<double>(size), k, aImp_original};
  return values;
}

int Population::getMinAgePrev() const { return minAgePrev; }

int Population::getMinAgeMDA() const { return minAgeMDA; }

void Population::setU(Host &h, double sigmaMDA, double sigmaBednets) {

  // create correlation matrix and mean for multivariate normal distribution.
  // correlation betqeen sys comp for bednets and MDA
  // called when applying either for the first ime or with a new sysComp value

  gsl_matrix *m = gsl_matrix_alloc(3, 3);
  gsl_matrix_set(m, 0, 0, 1.0);
  gsl_matrix_set(m, 0, 1, sigmaMDA * rhoBU);
  gsl_matrix_set(m, 1, 0, sigmaMDA * rhoBU);
  gsl_matrix_set(m, 1, 1, sigmaMDA * sigmaMDA);
  gsl_matrix_set(m, 2, 0, sigmaMDA * rhoCN * sigmaBednets);
  gsl_matrix_set(m, 0, 2, sigmaMDA * rhoCN * sigmaBednets);
  gsl_matrix_set(m, 2, 1, 0.0);
  gsl_matrix_set(m, 1, 2, 0.0);
  gsl_matrix_set(m, 2, 2, sigmaBednets * sigmaBednets);

  // set mean vector
  gsl_vector *v = gsl_vector_alloc(3);
  gsl_vector_set(v, 0, 0.0);
  gsl_vector_set(v, 1, u0CompMDA);
  gsl_vector_set(v, 2, u0CompBednets);

  // set result vector
  gsl_vector *result = gsl_vector_alloc(3);
  rmvnorm(3, v, m, result);

  h.uCompMDA = (double)gsl_vector_get(result, 1);
  h.uCompBednets = (double)gsl_vector_get(result, 2);

  gsl_matrix_free(m);
  gsl_vector_free(v);
}

void Population::rmvnorm(const int n, const gsl_vector *mean,
                         const gsl_matrix *var, gsl_vector *result) {
  /* multivariate normal distribution random number generator */
  /*
   * n dimension of the random vetor
   * mean  vector of means of size n
   * var variance matrix of dimension n x n
   * result  output variable with a sigle random vector normal distribution
   * generation
   */

  int k;
  gsl_matrix *work = gsl_matrix_alloc(n, n);

  gsl_matrix_memcpy(work, var);
  gsl_linalg_cholesky_decomp(work); // decompose variance

  for (k = 0; k < n; k++) // randomise result
    gsl_vector_set(result, k, stats.unit_normal_dist());

  gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, work,
                 result);       // result = result + A, A'
  gsl_vector_add(result, mean); // result = var*norm_distrib+mean

  gsl_matrix_free(work);
}

void Population::printMDAHistory() const {

  int minAgeMDAinMonths = minAgeMDA * 12;

  std::vector<int> bins;
  bins.assign(14, 0);

  for (int i = 0; i < size; i++) {

    if (host_pop[i].age >= minAgeMDAinMonths) {

      int num = host_pop[i].getNumMDAs();
      bins[num]++;
    }
  }
  std::cout << std::endl;
  for (int i = 0; i < bins.size(); i++)
    std::cout << i << " " << bins[i] << std::endl;
}

