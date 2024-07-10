//
//  Output.cpp
//  transfil
//
//  Created by Paul Brown on 24/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <sstream>
#include <iostream>

#include "Output.hpp"
#include "MDAEvent.hpp"
#include "Population.hpp"
#include "PrevalenceEvent.hpp"




void Output::initialise(){
    
    
    months.clear();
    months.reserve(100); //guess at likely max number of months that will be output
    
    
}


void Output::resetToMonth(int month){
    
    //This month may not have been saved if prevalence /mda not required
    //Just delete any months later than this one.

    
    while (months.size()){
        
        monthToOuput savedMonth = months.back();
        if (savedMonth.month < month)
            return;
        
        months.pop_back();
        
    }
    
}

Output& Output::operator=(const Output& op){
    
    //copies data to a scenario when done
    
    months = op.months;
    
    return *this;
}


std::string Output::printYearIndex(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printYearIndex: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    if (months[n].month < 0)
        return ""; //burn in
    else
        return std::to_string(floor(months[n].month/12));
    
}

std::string Output::printYear(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printYear: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    return std::to_string(int(floor(months[n].month/12) + baseYear));
    
}



std::string Output::printMonthIndex(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printMonthIndex: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
   
    
    if (months[n].month < 0)
        return ""; //burn in
    else
        return std::to_string(months[n].month);
    
}


std::string Output::printDate(int n) const {
    
    //formatted date output
    
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printDate: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    std::stringstream str;
    
    if (months[n].month >= 0){
        str << monthNames[months[n].month%12] << "-" << printYear(n);
        return str.str();
        
    }else
        return std::string("Init cond");
    
    
    
}

double Output::printBedNetCoverage(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printBedNetCoverage: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    return months[n].bedNetCov;
    
}

std::string Output::printBednetSysComp(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printBednetSysComp: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    if(months[n].bedNetCov>0)
        return std::to_string(months[n].bednetSysComp);
    else
        return "";
    
    
}

double Output::printAImpFactor(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printAImpFactor: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    return months[n].aImpFactor;
    
}

std::string Output::printMDACoverage(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printMDACoverage: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    if (months[n].mdaNeeded)
        return std::to_string(months[n].mdaCov);
    else
        return "";
    
}

std::string Output::printMDAType(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printMDACoverage: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    if (months[n].mdaNeeded)
        return months[n].mdaType;
    else
        return "";
    
}


std::string Output::printMDASysComp(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printMDASysComp: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    if (months[n].mdaNeeded)
        return std::to_string(months[n].mdaSysComp);
    else
        return "";

    
}

RecordedPrevalence* Output::printPrevalence(int n)  {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::printPrevalence: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    
    if(months[n].prevalenceNeeded)
        return &(months[n].prevalence);
    else 
        return NULL;
    //months exist in saveMonth called for this month
    //but prevaluence may not be needed if just mda applied
}


std::string Output::getMinAgeForTreatment(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::getMinAgeForTreatment: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    if(months[n].mdaNeeded)
        return std::to_string(months[n].minAgeMDA);
    else
        return "";
    
    
}

std::string Output::getMinAgeForPrevalence(int n) const {
    
    if (unsigned(n) >= months.size()){
        std::cout << "Error in Output::getMinAgeForPrevalence: Output requested beyond range of saved values." <<std::endl;
        exit(1);
    }
    if(months[n].prevalenceNeeded)
        return std::to_string(months[n].minAgePrev);
    else
        return "";
    

    
    
}




void Output::saveRandomNames(std::vector<std::string> names){
    
    for (unsigned i = 0; i < names.size(); i++)
        randomVarNames.push_back(names[i]);

}


void Output::saveRandomValues(std::vector<double> vals){
    
    //Values corresponding to names passed to above
    for (unsigned i = 0; i < vals.size(); i++)
        randomVarValues.push_back(vals[i]);
    
}


void Output::saveSeedValue(unsigned long int vals){
    
    //Values corresponding to names passed to above
    
    seedValue = vals;
    
}


int Output::getNumRandomVars(){
    
    //used for indenting results spreadsheet by right number of cols
    
    return int(randomVarNames.size());
    
}

std::string&  Output::getRandomVarNames(int idx){
    
    return randomVarNames[idx];
    
}

void Output::clearRandomValues(){
    
    randomVarValues.clear();
}

double  Output::getRandomVarValues(int idx){
    
    return randomVarValues[idx];
    
}

unsigned long int  Output::getSeedValue(){
    
    return seedValue;
    
}




void Output::saveMonth(int month, Population& popl, PrevalenceEvent* outputNeeded, RecordedPrevalence prevalence, MDAEvent* mda){
    
    //being saved because prevalence is due, or mda due, or both
    monthToOuput newMonth = monthToOuput(month, popl.getBedNetCoverage(), popl.getBedNetSysComp(),  popl.getImportationRateFactor());
    
    if(outputNeeded)
        newMonth.setPrevalence((outputNeeded->getMinAge() < 0)? popl.getMinAgePrev() : outputNeeded->getMinAge(), prevalence);
    if(mda)
        newMonth.setMDA(mda, popl.getMinAgeMDA());
    
    
    //also save popln structure at this timepoint
    //For now just hosts age 5+
   // newMonth.setAgeInRange();
    
    months.push_back(newMonth);
    
}


void monthToOuput::setMDA(MDAEvent* mda, int age){
    
    mdaCov = mda->getCoverage();
    mdaType = mda->getType();
    mdaSysComp = mda->getCompliance();
    minAgeMDA = (mda->getMinAge() < 0)? age : mda->getMinAge(); //unspecified so used default
    
    mdaNeeded = true;
    
}



void monthToOuput::setPrevalence(int age, RecordedPrevalence prev){
    
    prevalence = prev;
    prevalenceNeeded = true;
    minAgePrev = age;
}










