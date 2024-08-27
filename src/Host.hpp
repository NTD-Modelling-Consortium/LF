//
//  Host.hpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Host_hpp
#define Host_hpp

#include <string>

class Vector;
class Worm;

typedef struct {

  // state variables will be saved in this structure at the end of a run

  int WM, WF, totalWorms; // number of male worms, number of female worms
  double totalWormYears;  // number of male worms, number of female worms
  double M;               // mf concentration
  double biteRisk;
  double age;
  unsigned monthsSinceTreated;
  double hydroMult;
  double lymphoMult;
  int sex;
  double pTreat; // probability of treatment drawn from a beta distribution as
                 // defined in section 1.5.3 of supplement to
                 // https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5340860/
} hostState;

class Host {

  // operator to save a host
  //  friend hostState& operator<<(hostState& hs, const Host&);

public:
  void initialise(double deathRate, int maxAge, double k, double *totalBiteRisk,
                  double HydroceleShape, double LymphodemaShape,
                  double neverTreated); // initialise state variables.
  void reset(int age, double HydroceleShape, double LymphodemaShape,
             double neverTreated);
  void react(double dt, double deathRate, const int maxAge, double aImp,
             const Vector &vectors, const Worm &worms, double HydroceleShape,
             double LymphodemaShape, double neverTreated);
  void getsTreated(Worm &worms, std::string type);
  void restore(const hostState &state);
  int getNumMDAs() const { return numMDAs; };
  void initialisePTreat(double alpha, double beta);
  // state variables saved
  int WM, WF; // number of male worms, number of female worms
  int totalWorms;
  double totalWormYears;
  double M;                    // mf produced. //int
  double biteRisk;             // mean number of bites per month
  double age;                  // age of host.
  unsigned monthsSinceTreated; // host has been treated if
  double hydroMult;
  double lymphoMult;
  int sex; // 0 = male, 1 = female
  int neverTreat;
  double pTreat;
  bool bedNet;         // host uses bednet.
  double uCompBednets; // THe hosts probability of using a bednet for a given
                       // overall coverage
  double uCompMDA;
  // indicator whether the individual was infected at last time checked.
  // initialize at -1. Then will be set to 0 if uninfected and 1 if infected.
  // done at the start of each year
  int previouslyInfected;
  // operator to save a host
  operator hostState() const;

private:
  bool newImportation(double prob);
  bool hostDies(double prob);
  int numMDAs;
};

#endif /* Host_hpp */
