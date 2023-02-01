//
//  ScenariosList.cpp
//  transfil
//
//  Created by Paul Brown on 28/02/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits.h>
#include "ScenariosList.hpp"
#include "BedNetEvent.hpp"
#include "PrevalenceEvent.hpp"
#include "ImportationRateEvent.hpp"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "Population.hpp"

extern bool _DEBUG;



const Scenario& ScenariosList::getScenario(int snum) const {
    
    //get required interventions for this scenario
    
    
    return scenarios[snum]; 
    
}

unsigned long ScenariosList::getNumScenarios() const {
    
    return scenarios.size();
    
}

//Region specific scenarios

void ScenariosList::createScenarios(TiXmlElement* xmlListOfScenarios, const std::string opDir){
    
    //create a list of the different scenarios
    
    //has attributes, 'name', 'start' and 'end'
    int finalYear = baseYear = 0;
    extraMinAge = 0;
    extraMaxAge = Population::getMaxAge();
    
    outputMethod.assign(""); //default is empty string meaning all  3

    xmlListOfScenarios->Attribute("start", &baseYear);
    xmlListOfScenarios->Attribute("end", &finalYear);
    name.assign(xmlListOfScenarios->Attribute("name"));
    if(xmlListOfScenarios->Attribute("method"))
        outputMethod.assign(xmlListOfScenarios->Attribute("method"));
    
    if(finalYear <= 0 || finalYear <= baseYear){
        std::cout << "Error in ScenariosList::createScenarios. The scenario list start and end years are not properly defined." <<std::endl;
        exit(1);
    }
    
    
    numYears = (finalYear - baseYear + 1);
    numMonths = numYears*12;
    
    //child 'extraOutput'
    
    TiXmlElement* xmlExtra = NULL;
    
    if ((xmlExtra = xmlListOfScenarios->FirstChildElement("extraOutput"))){
        
        if (!xmlExtra->Attribute("minage", &extraMinAge))
            extraMinAge = 0;
        
        if (!xmlExtra->Attribute("maxage", &extraMaxAge))
            extraMaxAge = Population::getMaxAge();
        
    }
    
    
    TiXmlElement* xmlScenario = NULL;
    
    for ( xmlScenario = xmlListOfScenarios->FirstChildElement("scenario"); xmlScenario != NULL; xmlScenario = xmlScenario->NextSiblingElement("scenario")){

        //for each scenario
        std::string sname;
        int sMonth = 0;
        std::vector<BedNetEvent> bedNets;
        std::vector<MDAEvent> mdaEvents;
        std::vector<PrevalenceEvent> outputEvents;
        std::vector<ImportationRateEvent> aImpEvents;
        
        
        //has attributes 'name' and 'start'
        sname.assign(xmlScenario->Attribute("name"));
        if (!xmlScenario->Attribute("start", &sMonth))
            sMonth = 0; //not defined, default 0
        
        
        if(scenarios.empty() && sMonth > 0){
            std::cout << "Error in ScenariosList::createScenarios. The first scenario must begin at month zero." <<std::endl;
            exit(1);
        }else if (sMonth < 0){
            sMonth = 0;
        }else if (sMonth >= numMonths){
            std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " start month is beyond the end of the simulation." << std::endl;
            exit(1);
        }

        
        
        //has child 'output' with attributes 't' and 'minage'
        TiXmlElement* xmlOutput = NULL;
        
        for ( xmlOutput = xmlScenario->FirstChildElement("output"); xmlOutput != NULL; xmlOutput = xmlOutput->NextSiblingElement("output")){
            
            int t = -1;
            int minage = -1;
            if (!xmlOutput->Attribute("t", &t) || t < 0){
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " output t is not defined." <<std::endl;
                exit(1);
            }else if (t < sMonth) {
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " output t " << t << " earlier than start of scenario." <<std::endl;
                exit(1);
                
            }
            if(!xmlOutput->Attribute("minage", &minage))
                minage = -1; //default use values from params file for main output files
            
            
            outputEvents.push_back(PrevalenceEvent(t, minage, extraMinAge, extraMaxAge, outputMethod));
            //ignore any before start year
        
            
        }
        
        if (outputEvents.empty()){
            std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " defines no outputs." <<std::endl;
            exit(1);
        }

        
        //child mda
        
        TiXmlElement* xmlMDA = NULL;
        
        for ( xmlMDA = xmlScenario->FirstChildElement("mda"); xmlMDA != NULL; xmlMDA = xmlMDA->NextSiblingElement("mda")){
            
             //attributes 't' ,'type', 'coverage', 'syscomp', 'minage'
            int t = -1;
            int minage = -1;
            double mdacov = 0.0, sysComp = -1;
            std::string mdaType;
            
            if (!xmlMDA->Attribute("t", &t) || t < 0){
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " mda t is not defined." <<std::endl;
                exit(1);
            }else if (t < sMonth) {
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " mda t " << t << " earlier than start of simulation." <<std::endl;
                exit(1);
                
            }
            
            if(!xmlMDA->Attribute("minage", &minage))
                minage = -1; //default use values from params file
            
            xmlMDA->Attribute("coverage", &mdacov);
            xmlMDA->Attribute("syscomp", &sysComp);
            mdaType.assign(xmlMDA->Attribute("type"));
            
            if (mdacov <= 0 || mdacov > 1.0){
                std::cout << "Error in ScenariosList::createScenarios, scenario " << sname << ". MDA coverage must be > 0 and <= 1.0" << std::endl;
                exit(1);
            }
            if (sysComp < 0 || sysComp >= 1.0){
                std::cout << "Error in ScenariosList::createScenarios , scenario " << sname << ". Systematic compliance for MDA must be >= 0 and < 1" << std::endl;
                exit(1);
            }
            
            if (mdaType.empty()){
                std::cout << "Error in ScenariosList::createScenarios, scenario " << sname << ". MDA type is not defined" << std::endl;
                exit(1);
            }
            
            mdaEvents.push_back(MDAEvent(t, minage, mdacov, sysComp, mdaType));
  
          
        }
        
        //child 'bednet'
        TiXmlElement* xmlBedNet = NULL;
        int firstMonthBednets = numMonths;
        
        for ( xmlBedNet = xmlScenario->FirstChildElement("bednet"); xmlBedNet != NULL; xmlBedNet = xmlBedNet->NextSiblingElement("bednet")){
            
            
             //attributes 'index', 'coverage', 'syscomp'
            int t = -1;
            double  sysComp = -1;
            double bednetCov = 0.0;
            
            if (!xmlBedNet->Attribute("t", &t) || t < 0){
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << ". Bed net t is not defined." <<std::endl;
                exit(1);
            }else if (t < sMonth) {
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " bed net month " << t << " earlier than start of simulation." <<std::endl;
                exit(1);
                
            }

            
            xmlBedNet->Attribute("coverage", &bednetCov);
            xmlBedNet->Attribute("syscomp", &sysComp);
            
            if (bednetCov < 0 || bednetCov > 1.0){
                std::cout << "Error in ScenariosList::createScenarios, scenario " << sname << ". Bed net coverage must be >= 0 and <= 1.0" << std::endl;
                exit(1);
            }
            if (sysComp < 0 || sysComp >= 1.0){
                std::cout << "Error in ScenariosList::createScenarios, scenario " << sname << ". Systematic compliance for bed nets must be >= 0 and < 1" << std::endl;
                exit(1);
            }
            
            bedNets.push_back(BedNetEvent(t,bednetCov, sysComp));
            
            firstMonthBednets = (t < firstMonthBednets)? t:firstMonthBednets;

        }
        
        //prepend object representing zero bednets in case user didn't specify value at month zero.
        if (firstMonthBednets > 0)
            bedNets.insert(bedNets.begin(), BedNetEvent(0, 0, 0.99));
        
        
        //importatio rate factor
        TiXmlElement* xmlImp = NULL;
        double firstMonthaImp = numMonths;
        
        for ( xmlImp = xmlScenario->FirstChildElement("importation"); xmlImp != NULL; xmlImp = xmlImp->NextSiblingElement("importation")){
            
            int t = -1;
            double aImp = -1;
            if (!xmlImp->Attribute("t", &t) || t < 0){
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " importation t is not defined." <<std::endl;
                exit(1);
            }else if (t < sMonth) {
                std::cout << "Error in ScenariosList::createScenarios. Scenario " << sname << " importation t " << t << " earlier than start of scenario." <<std::endl;
                exit(1);
                
            }
            if(!xmlImp->Attribute("value", &aImp))
                aImp = 1; //default use
            
            aImpEvents.push_back(ImportationRateEvent(t, aImp));
            //ignore any before start year
            firstMonthaImp = (t < firstMonthaImp)? t:firstMonthaImp;
            
        }
        
        if (firstMonthaImp > 0)
            aImpEvents.insert(aImpEvents.begin(), ImportationRateEvent()); //default value of 1 for start of simulation
        
        
        //done, create scenario object
        scenarios.push_back(Scenario(numMonths, sMonth, sname, bedNets, mdaEvents, outputEvents, aImpEvents));
        
        if(_DEBUG)
            std::cout << "Scenario " << sname << " starts moth " << sMonth  << ": " << mdaEvents.size() << " MDA events, Prevalence is output " << outputEvents.size() << " times."<<  std::endl;
        
  
        
    } //next scenario
    
    if(!scenarios.size()){
        std::cout << "Error in ScenariosList::createScenarios. No scenarios found in file." <<std::endl;
        exit(1);
    }
    
    

    //Now calculate the months that we need to save as each scenario runs. This is done as follows
    //1) For scenario n, examine the start month for scenarios n+1 ... end
    //2) if their start month is both later than the start month for scenario n AND earlier than any other month that scenario n will save THEN
    // save this month when running scenario n.
    //3) If this month has a start month earlier than scenario n, then stop
    
    for (unsigned s = 0; s < scenarios.size(); s++){
        
        std::vector<int> toSave;
        int currentStartMonth = scenarios[s].getStartMonth();
        int earliestMonthSaved = INT_MAX;
        
        for (unsigned t = s+1; t < scenarios.size(); t++){
            
            int newStartMonth = scenarios[t].getStartMonth();
            
            if (newStartMonth <= currentStartMonth)
                break;
            else if (newStartMonth < earliestMonthSaved){

                toSave.push_back(newStartMonth);
                earliestMonthSaved = (earliestMonthSaved < newStartMonth)?earliestMonthSaved:newStartMonth;
            }
            
        }
        toSave.push_back(numMonths-1);
        std::sort(toSave.begin(), toSave.end());
        scenarios[s].setMonthsToSave(toSave);

        if(_DEBUG){
            std::cout << scenarios[s].getName() << " will save months :";
            for (unsigned i =0; i < toSave.size(); i++)
                std::cout << " " << toSave[i];
            std::cout << std::endl;
            
        }
        
    }
    
    outputDir = opDir;

    
}


void ScenariosList::openFilesandPrintHeadings(int index, Output& results) {
    
    for(unsigned s = 0; s < scenarios.size(); s++){
        std::stringstream fname;
        fname << outputDir << index << "_" << name;
        scenarios[s].openFileandPrintHeadings(fname.str(), results) ;
    }
    
}


void ScenariosList::closeFiles(){
    
    for(unsigned s = 0; s < scenarios.size(); s++)
        scenarios[s].closeFile();
}



