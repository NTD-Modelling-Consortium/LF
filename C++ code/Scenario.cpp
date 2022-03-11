//
//  Scenario.cpp
//  transfil
//
//  Created by Paul Brown on 02/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "Scenario.hpp"
#include "Output.hpp"
#include <sstream>




void Scenario::updateBedNetCoverage(Population& popln, int month){
    
    popln.updateBedNetCoverage(getBedNetCoverage(month));
    
}

void Scenario::updateImportationRate(Population& popln, int month){
    
    
    // This has effect on Vector::L3 below and also on host::react
 
    popln.updateImportationRate(getImportationFactor(month));

    
}

BedNetEvent* Scenario::getBedNetCoverage(int month) const{
    
    //If NULL pointer, go back to the last month it was defined. bedNetCoverage[0] guaranteed not to be null
    BedNetEvent* bn = NULL;
    while(!(bn = bedNetCoverage[month]))
        month--;
    
    return bn; //never NULL
    
}

double Scenario::getImportationFactor(int month) const{
    
    ImportationRateEvent* aImpF = NULL;
    while (!(aImpF = aImpUpdate[month]))
        month--;
        
        
    return aImpF->getFactor();
    
}


MDAEvent* Scenario::treatmentDue(int month) const {
    
    //cale each month by Model::evovleAndSAve()
    //month is zero based month number from start of simulation
    
    return mdaDue[month]; //NULL if not due
    
}

PrevalenceEvent* Scenario::prevalenceDue(int month) const {
    
    return outputDue[month]; //NULL if not due
    
}



void Scenario::setMonthsToSave(std::vector<int>& mnts){
    
    monthsToSave = mnts;
    
}


int Scenario::getStartMonth() const{
    
    return startMonth;
}

int Scenario::getNumMonthsToSave() const {
    
    return int(monthsToSave.size()); //allows us to go back in time to re-use data
}

int Scenario::getMonthToSave(int idx) const {
    
    return monthsToSave[idx];
}


void  Scenario::openFileandPrintHeadings(std::string region, Output& results){
    
    std::string fname;
    
    popFiles = new std::ofstream[popAgeRanges.size()];
    
    //pop size file(s)
    for (int i = 0; i < popAgeRanges.size(); ++i){
     
        fname = region + "_" + name + "_Pop_" + std::to_string(popAgeRanges[i]) + ".txt";
        
        popFiles[i].open(fname, std::ios_base::out);
        if (!popFiles[i].is_open()){
                   
            std::cout << "Error writing results to file " << fname << std::endl;
            exit(1);
                   
        }
        
        popFiles[i] << name << "\tHosts in population aged ";
        if (i == popAgeRanges.size()-1) //last age range
              popFiles[i] << std::to_string(popAgeRanges[i]) << " years and above" << std::endl;
        else
             popFiles[i] << "between" << std::to_string(popAgeRanges[i]) << " and " << std::to_string(popAgeRanges[i+1]-1) << " years" << std::endl;
        

    }
    
    
    
    
    if (ICNeeded){
        
        fname = region + "_" + name + "_IC.txt";
         myFileIC.open(fname, std::ios_base::out);
        if (!myFileIC.is_open()){
            
            std::cout << "Error writing results to file " << fname << std::endl;
            exit(1);
            
        }
        myFileIC << name << "\tPevalence calculated using IC method" << std::endl;
        
    }
    
    if (MFNeeded){
        
        fname = region + "_" + name + "_MF.txt";
        myFileMF.open(fname, std::ios_base::out);
        if (!myFileMF.is_open()){
            
            std::cout << "Error writing results to file " << fname << std::endl;
            exit(1);
            
        }
        myFileMF << name << "\tPevalence calculated using MF method" <<std::endl;
        
    }
    
    if (WCNeeded){
           
           fname = region + "_" + name + "_WC.txt";
           myFileWC.open(fname, std::ios_base::out);
           if (!myFileWC.is_open()){
               
               std::cout << "Error writing results to file " << fname << std::endl;
               exit(1);
               
           }
           myFileWC << name << "\tPevalence calculated using Worm count method" <<std::endl;
           
       }
    
    
    //do we need extra file for extra age-restriced outputs?
    if (requiresExtra){
        
        std::string fnameExtra = region + "_" + name + "_age_";
        fnameExtra += std::to_string(minAgeExtra);
        fnameExtra += "_";
        fnameExtra += std::to_string(maxAgeExtra);
        
        if (ICNeeded){
            
            fname = fnameExtra + "_IC.txt";
            myFileExtraIC.open(fname, std::ios_base::out);
            if (!myFileExtraIC.is_open()){
                
                std::cout << "Error writing results to file " << fname << std::endl;
                exit(1);
                
            }
            myFileExtraIC << name << "\tPevalence calculated using IC method\tHosts aged between " << minAgeExtra << " and " << maxAgeExtra << " years" << std::endl;
            
        }
        
        if (MFNeeded){
            
            fname = fnameExtra + "_MF.txt";
            
            myFileExtraMF.open(fname, std::ios_base::out);
            if (!myFileExtraMF.is_open()){
                
                std::cout << "Error writing results to file " << fname << std::endl;
                exit(1);
                
            }
            myFileExtraMF << name << "\tPevalence calculated using MF method\tHosts aged between " << minAgeExtra << " and " << maxAgeExtra << " years" << std::endl;
        }
        
        if (WCNeeded){
            
            fname = fnameExtra + "_WC.txt";
            
            myFileExtraWC.open(fname, std::ios_base::out);
            if (!myFileExtraWC.is_open()){
                
                std::cout << "Error writing results to file " << fname << std::endl;
                exit(1);
                
            }
            myFileExtraWC << name << "\tPevalence calculated using Worm count method\tHosts aged between " << minAgeExtra << " and " << maxAgeExtra << " years" << std::endl;
        }
        
    }
     
}


void Scenario::printResults(int repnum, Output& results, Population& popln){
    
    //caled at the end of each replicate
    
    if (ICNeeded){
        if(!repnum) printColumnTitles(myFileIC, results);
        printReplicate(myFileIC, results, repnum, popln); //print randaom values used in this replicate
    }
    if (MFNeeded){
        if(!repnum) printColumnTitles(myFileMF,  results);
        printReplicate(myFileMF, results, repnum, popln);
    }
    if (WCNeeded){
        if(!repnum) printColumnTitles(myFileWC,  results);
        printReplicate(myFileWC, results, repnum, popln);
    }
    
    for (int i = 0 ; i < popAgeRanges.size(); i++) {
        if(!repnum) printColumnTitles(popFiles[i], results);
        printReplicate(popFiles[i], results,repnum, popln);
    }
   

    if (requiresExtra){
            
        if (ICNeeded){
            if(!repnum) printColumnTitles(myFileExtraIC, results, true);
            printReplicate(myFileExtraIC, results, repnum, popln);
        }
        if (MFNeeded){
             if(!repnum) printColumnTitles(myFileExtraMF, results, true);
            printReplicate(myFileExtraMF, results, repnum, popln);
        }
        if (WCNeeded){
             if(!repnum) printColumnTitles(myFileExtraWC, results, true);
            printReplicate(myFileExtraWC, results, repnum, popln);
        }
    }
    
    
    //print prevalence for each month with either mda or output (see Model::evolveAndSave())
    

    for (int m = 0; m < results.getSize(); m++){
        
        //For each month
        
        RecordedPrevalence* prevalence = results.printPrevalence(m);
        //this will be null if output month exists only due to mda
        
        //pop size
        
        for (int i = 0 ; i < popAgeRanges.size(); i++) {
            
            if (prevalence) {
                
                int lower = popAgeRanges[i];
                int upper = ( i < (popAgeRanges.size()-1))? popAgeRanges[i+1]-1 : -1;
                popFiles[i] << "\t" << prevalence->getAgeInRange(lower, upper); //range is inclusive
                
            }else
                popFiles[i] << "\t "; //placeholder, just mda needed for this month
                
        }
        
     
        
        if (ICNeeded) {
            if (prevalence)
                myFileIC << "\t" << prevalence->IC;
            else
                myFileIC << "\t" << " "; //placeholder, just mda needed for this month
        }
        
        if (MFNeeded) {
             if (prevalence)
                 myFileMF << "\t" << prevalence->MF;
            else
                myFileMF << "\t" << " ";
        }
        
        if (WCNeeded) {
            if (prevalence)
                myFileWC << "\t" << prevalence->WC;
            else
                myFileWC << "\t" << " ";
        }
        
         if(requiresExtra){
             
             if (ICNeeded) {
                 
                 if (prevalence)
                    myFileExtraIC << "\t" << prevalence->ICRestrictedAge;
                else
                    myFileExtraIC << "\t" << " "; //placeholder, just mda needed for this month
             
             }
             
             if (MFNeeded) {
                 
                  if (prevalence)
                      myFileExtraMF << "\t" << prevalence->MFRestrictedAge;
                 else
                     myFileExtraMF << "\t" << " ";
             }
             
             if (WCNeeded) {
                 
                  if (prevalence)
                      myFileExtraWC << "\t" << prevalence->WCRestrictedAge;
                 else
                     myFileExtraWC << "\t" << " ";
             }
             
         }
        
    }
    

    if (ICNeeded){
        myFileIC << "\t" << popln.totMDAs;
        myFileIC << "\t" << popln.post2020MDAs;
        myFileIC << "\t" << popln.numPreTASSurveys;
        myFileIC << "\t" << popln.numTASSurveys;
        myFileIC << "\t" << popln.t_TAS_Pass;
        myFileIC << std::endl;
    }
        
    if (MFNeeded){
        myFileMF << "\t" << popln.totMDAs;
        myFileMF << "\t" << popln.post2020MDAs;
        myFileMF << "\t" << popln.numPreTASSurveys;
        myFileMF << "\t" << popln.numTASSurveys;
        myFileMF << "\t" << popln.t_TAS_Pass;
        myFileMF << std::endl;
    }
        
    if (WCNeeded){
        myFileWC << "\t" << popln.totMDAs;
        myFileWC << "\t" << popln.post2020MDAs;
        myFileWC << "\t" << popln.numPreTASSurveys;
        myFileWC << "\t" << popln.numTASSurveys;
        myFileWC << "\t" << popln.t_TAS_Pass;
        myFileWC << std::endl;
    }
       
    
    if(requiresExtra){
        if (ICNeeded)
            myFileExtraIC << std::endl;
        if (MFNeeded)
            myFileExtraMF << std::endl;
        if (WCNeeded)
            myFileExtraWC << std::endl;
        
    }
    
     for (int i = 0 ; i < popAgeRanges.size(); i++)
         popFiles[i] << std::endl;
    
}


void Scenario::printColumnTitles(std::ofstream& of, Output& results, bool extra) const {
    
    std::string indent(results.getNumRandomVars()+1, '\t');
    
    //print headers for every month with either prevalenc or mda
    
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
    
    of << indent <<  "Bed net coverage";
    for (int m = 0; m < results.getSize(); m++)
        of << "\t" << results.printBedNetCoverage(m);
    of << std::endl;
    of << indent <<  "Bed net systemtic compliance";
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
    
    if (extra){
         of << indent << "Age range for prevalance calculation";
        for (int m = 0; m < results.getSize(); m++)
            of << "\t" << minAgeExtra << " to " << maxAgeExtra;
        of << std::endl;
        
    }else{
        
        of << indent << "Minimum age for prevalence calculation";
        for (int m = 0; m < results.getSize(); m++)
            of << "\t" << results.getMinAgeForPrevalence(m);
        of << std::endl;
    }
    
    of << "ID";
    
    for (int i =0; i < results.getNumRandomVars(); i++)
        of << "\t" << results.getRandomVarNames(i);
    of << std::endl;
    
}


void Scenario::printReplicate(std::ofstream& of, Output& results, int repnum, Population& popln) const {
    
    
    of << (repnum+1);
    for (int i =0; i < results.getNumRandomVars(); i++)
        if (results.getRandomVarValues(i) >= 0.0)
            of << "\t" << results.getRandomVarValues(i);
        else
            of << "\t" << "na"; //this value wasn't used
            
    of << "\t";
    
    
}



void Scenario::closeFile(){
    
    if (ICNeeded)
        myFileIC.close();
    if (MFNeeded)
        myFileMF.close();
    if (WCNeeded)
        myFileWC.close();
    
    
    if (requiresExtra){
        
        if (ICNeeded)
            myFileExtraIC.close();
        if (MFNeeded)
            myFileExtraMF.close();
        if (WCNeeded)
            myFileExtraWC.close();
    }
    
    
    for (int i = 0 ; i < popAgeRanges.size(); i++)
        popFiles[i].close();
    
    delete[] popFiles;
    
}

bool Scenario::requiresIC() const {
    
    return ICNeeded;

    
}

bool Scenario::requiresMF() const {
    
    return MFNeeded;
    
    
}

bool Scenario::requiresWC() const {
    
    return WCNeeded;
    
    
}










