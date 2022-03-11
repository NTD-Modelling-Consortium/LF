//
//  Population.hpp
//  transfil
//
//  Created by Paul Brown on 03/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Population_hpp
#define Population_hpp

#include <iostream>
#include <string>
#include <random>
#include <map>
#include "Host.hpp"
#include "Vector.hpp"
#include "Statistics.hpp"
#include "RecordedPrevalence.hpp"


//class Vector;
class Worm;
class MDAEvent;
class PrevalenceEvent;
class BedNetEvent;
class TiXmlElement;

#define MAX_POP 11550

 

//class to represent a population
//A container class for the Host class and also provides a size distribution function


class Population {
    
    friend std::ostream& operator<<(std::ostream& ostr, const Population&);
    
public:
    
    Population(TiXmlElement* xmlParameters);
    
    ~Population(){
        if (popSize != NULL)
            delete[] popSize;
        if(populationDistribution != NULL)
            delete populationDistribution;
        
        
    }
    
    void loadPopulationSize(const std::string filename);
    static int getMaxAge();
    void initHosts(std::string distType, double k_val, double aImp_val);
    double getPopSize() ;
   
    RecordedPrevalence  getPrevalence(PrevalenceEvent* outputPrev)  const;
    
    double getLarvalUptakebyVector(double r1, double kappas1, Vector::vectorSpecies species) const;
    double getBedNetCoverage()  const;
    double getBedNetSysComp() const ;
    double getImportationRateFactor()  const;
    double getMFPrev();
    double getICPrev();

    void updateBedNetCoverage(BedNetEvent* bn);
    void updateImportationRate(double factor);
    
    void evolve(double dt, const Vector& vectors, const Worm& worms);
    void ApplyTreatment(MDAEvent* mda,  Worm& worms);
    void saveCurrentState(int month, std::string sname);
    void resetToMonth(int month);
    void clearSavedMonths();
    
    std::vector<std::string> printRandomVariableNames() const;
    std::vector<double> printRandomVariableValues() const;
    int getMinAgePrev() const;
    int getMinAgeMDA() const;
    
     void printMDAHistory() const;
    
    int surveyStartDate;
    double ICThreshold;
    double MFThreshold;
    int interSurveyPeriod;
    int firstTASNumMDA;
    int numPreTASSurveys = 0;
    int numTASSurveys = 0;
    int totMDAs = 0;
    int post2020MDAs = 0;
    int t_TAS_Pass = -1;
    double aImp;
private:
    
    double calcU0(double coverage, double sigma);
    void setU(Host& h, double sigmaMDA, double sigmaBednets);
    void rmvnorm(const int n, const gsl_vector *mean, const gsl_matrix *var, gsl_vector *result);
    
    //param sampling
    std::default_random_engine randgen;
    std::discrete_distribution<int>* populationDistribution;
    int* popSize;
    
    //The hosts
    Host host_pop[MAX_POP];
    
    int size;
    //int sizeExtra;
    
    const static int maxAge = 100;
    double tau; // death rate of host    

   
    double sysCompMDA;
    double sysCompBednets;
    
    
    double u0CompBednets;     //Need to set Host::uCompN for bednet compliance. This is the mean of a normal distribution from which a random value is take to determine whether host complies or not
                        //Set to a value that will give a probability of compliance equal to the specified coverage covN
    double u0CompMDA;
   
    
    double k;     //shape parameter for gamma distrib used to get biteRisk
    
    double rhoBU;   //correlation between non-compliance and bite risk
    double rhoCN;   //correlation of non-compliance between bed-nets and MDA (Chemotherapy)
    
    double TotalBiteRisk; //sum for whole population
    
    
    //max values for random parameters
    double mink;
    double maxk;
    double maxaImp;
    double meanaImp;
    
    
    double aImp_original;
    double aImp_factor;
    double bedNetCov;       //the value when syscomp last changed, ie the value used ot calculate hosts' u values
    double mdaCoverage;
   
    
    int minAgePrev; //min age to include in prevalance clac
    
    int minAgeMDA; //min age to receive MDA
   
    int minAgeMF; //min age to be tested for MF
    int maxAgeMF; //max age to be tested for MF
    int minAgeIC; //min age to be tested for antigen
    int maxAgeIC; //max age to be tested for antigen
    double ICsensitivity; // sensitivity of antigen test
    
    //saved months
    
    
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
        std::string scenario; //for debugging only. The scenario that saved this month
        
    } savedMonth;
    
    std::vector<savedMonth> savedMonths;

    
};


#endif /* Population_hpp */
