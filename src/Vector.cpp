//
//  Vector.cpp
//  transfil
//
//  Created by Paul Brown on 08/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>

#include "Population.hpp"
#include "Worm.hpp"
#include "Vector.hpp"
#include "Statistics.hpp"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

extern Statistics stats;

Vector::Vector(TiXmlElement* xmlParameters){
    
    
    int gotAll = 0;
    
    TiXmlElement* xmlVector = xmlParameters->FirstChildElement("vector");
    
    if (xmlVector == NULL){
        std::cout << "Error in Vector::Vector. Cannot find vector parameters." << std::endl;
        exit(1);
        
    }
    
    TiXmlElement* xmlParam = NULL;
    
    for ( xmlParam = xmlVector->FirstChildElement("param"); xmlParam != NULL; xmlParam = xmlParam->NextSiblingElement("param")){
        
        std::string name;
        double value;
        name.assign(xmlParam->Attribute("name"));
        xmlParam->Attribute("value", &value);
        
        if (name == "species"){
            if (int(value) == 0)
                species = Anopheles;
            else
                species = Culex;
            gotAll++;
        }else if (name == "L3"){
            initL3 = value;
            L3 = initL3;
            gotAll++;
        }else if (name == "lambda"){
            lambda = value;
            gotAll++;
        }else if (name == "g"){
            g = value;
            gotAll++;
        }else if (name == "kappas1"){
            kappas1 = value;
            gotAll++;
        }else if (name == "r1"){
            r1 = value;
            gotAll++;
        }else if (name == "sigma"){
            sigma = value;
            gotAll++;
        }else
            std::cout << "Unknown parameter " << name << " in Vector parameter list." << std::endl;
    }
    
    
    if(gotAll < 7){
        std::cout << "Error in Vector::Vector. File  does not contain all the required parameters."<< std::endl;
        exit(1);
    }else
         std::cout << "Vector parameters: species=" << species <<", L3=" << L3 << ", lambda=" << lambda << ", g=" << g << ", kappas1=" << kappas1 << ", r1=" << r1 << ", sigma=" << sigma << std::endl<< std::endl;

    
}


void Vector::reset(std::string distType, double v_to_h_val){
    
    //generat new v to h ratio
    
  //  if(distType == "uniform")
  //      v_to_h = (stats.uniform_dist() * maxRatio); //must be in range 0 <= v_to_h <= 53
  //  else
  //      while((v_to_h = stats.exp_dist(meanRatio)) > maxRatio);
    
    //reset L3
    L3 = initL3;
    
    //tmp fix
    v_to_h =  v_to_h_val;
    
    
    
}

void Vector::updateVtoH(double v_to_h_val){
    v_to_h =  v_to_h_val;

}

void Vector::updateL3Density(const Population& popln, const Worm& worms){
    
    //called each month, average number of L3 larvae taken up by each mosquito
    
    //Av larvae per mosquito calculated by generating
    //1) a rate of uptake of mf from each host which is a function of mf conc in host
    //2) a weighted mean of these values, weighted according to the hosts bite risk
    
    double mfUptake = popln.getLarvalUptakebyVector(r1, kappas1, species);//mf is number taken up by entire vector population this month
    
    //return equilibrium number reduced according to bednet coverage
    
    // (1 - 0.97cov) * biterate * propmosquitosinfectedbybite / (mosquitodeatthrate + biterate * propL3leavingmosquitoperbite)
    
    double bedNetCoverage = popln.getBedNetCoverage();
    //bednets kill vector. This rate is effectively the worm death rate as their lifespan much longer than mosquitos
    double deathrate = sigma + lambda * dN * bedNetCoverage;
    
    //bednets not 100% effective, eg if sN = 0.03, 30% coverage becomes 0.3 * (1-0.03) = 29% effective coverage
    bedNetCoverage *= (1-sN);
    
    //equilibrium is mean uptake   * num bites per mosquito * prop picking up infection / (death rate + num bites * prop l3 leaving per bite)
    L3 =  mfUptake * ( 1.0 - bedNetCoverage ) * lambda * g /(deathrate + lambda * worms.getPropLeavingVectorPerBite());
    
    //L3=0.01;
    
}

double Vector::getL3Density() const {
    
    return L3;
}

double Vector::averageNumBites() const {
    
    //This is average bit rate per mosquito multipied by total numbr moquitos per host
    //to give average bites per host per month
    
    return lambda * v_to_h;
    
}



double Vector::probBitesThroughNet() const {
    
    //probability that a vector can successfully feed on a host using a bednet
    return sN;
}


void Vector::saveCurrentState(int month){
    
    //save larval density as this changes month to month
    savedMonths.push_back({month, L3});
    
}

void Vector::resetToMonth(int month){
    
    //move back in time, deleting saved months coming after the target month

    
    while(savedMonths.size()){
        
        savedMonth lastMonth = savedMonths.back();
        
        if (lastMonth.month == month){
            
            //restore larval density
            L3 = lastMonth.larvalDensity;
            return;
        }
        savedMonths.pop_back();
            
    }
        
    std::cout << "Error in Vector::resetToYear. Cannot find the specified month " << month << std::endl;
    exit(1);
    
    
}

void Vector::clearSavedMonths(){
    
    savedMonths.clear();
    
}

//print random variables

std::ostream& operator<<(std::ostream& ostr, const Vector& vec){
    
    return ostr << vec.v_to_h;
    
}

std::vector<std::string> Vector::printRandomVariableNames() const {
    
    //outputs a list of name/value pairs of any length. These are passed to output object to be printed to file
    
    std::vector<std::string> names = {"V to H ratio"};
    return names;
    
}

std::vector<double> Vector::printRandomVariableValues() const {
    
    //outputs a list of name/value pairs of any length. These are passed to output object to be printed to file
    
    std::vector<double> values = {v_to_h};
    return values;
    
}

