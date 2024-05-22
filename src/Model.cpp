//
//  Model.cpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "Model.hpp"
#include "ScenariosList.hpp"
#include "Scenario.hpp"
#include "Population.hpp"
#include "Vector.hpp"
#include "Worm.hpp"
#include "RecordedPrevalence.hpp"



extern bool _DEBUG;
extern Statistics stats;


void Model::runScenarios(ScenariosList& scenarios, Population& popln, Vector& vectors, Worm& worms, int replicates, double timestep, int index, std::string randParamsfile, std::string opDir){
    
    std::cout << std::endl << "Index " << index << " running " << scenarios.getName() << " with " << scenarios.getNumScenarios() << " scenarios" << std::endl;
    
    std::cout << std::unitbuf;
    std::cout << "Progress:  0%";
    
    Output currentOutput(scenarios.getBaseYear());
    currentOutput.saveRandomNames(popln.printRandomVariableNames());
    currentOutput.saveRandomNames(vectors.printRandomVariableNames()); //names of random vars to be printed
    currentOutput.saveRandomNames(worms.printRandomVariableNames());
    
    dt = timestep;
    
    scenarios.openFilesandPrintHeadings(index, currentOutput);
    
    
    //Temporary fix. Read file containing random values for v_to_h, aImp and k
    //bool updateParams = true;
    //bool updateParams = false;
    std::vector<double> k_vals;
    std::vector<double> v_to_h_vals;
    std::vector<double> aImp_vals;
    std::vector<double> wPropMDA;
    
    //getRandomParameters(index, k_vals, v_to_h_vals, aImp_vals, wPropMDA, unsigned(replicates), randParamsfile);
    
    for (int rep = 0; rep < replicates; rep++){
        getRandomParametersMultiplePerLine(rep+1, k_vals, v_to_h_vals, aImp_vals, wPropMDA, unsigned (replicates), randParamsfile) ;
        currentMonth = 0;
        popln.clearSavedMonths();
        vectors.clearSavedMonths();
        currentOutput.initialise(); //delete previous replicate
        
        //functions temporarily altered to allow random value to be passed in
        
        std::string distType = stats.selectDistribType();
        //create a new host population with random size, ages and compliance p vals and bite risk. Generates new value for k and aImp
        //popln.initHosts(distType, k_vals[rep], aImp_vals[rep]);
        popln.initHosts(distType, k_vals[0], aImp_vals[0]);
        //generate vector to host ratio value and reset L3 to initial value
        //vectors.reset(distType, v_to_h_vals[rep]);
        vectors.reset(distType, v_to_h_vals[0]);
        
        //tmp fix to set wPropMDA
        //worms.reset(wPropMDA[rep]);
        worms.reset(wPropMDA[0]);
        
        
        //save these values for printing later
        currentOutput.clearRandomValues();
        //MUST be cvalled i nsame order as saveRandomNames above
        currentOutput.saveRandomValues(popln.printRandomVariableValues());
        currentOutput.saveRandomValues(vectors.printRandomVariableValues());
        currentOutput.saveRandomValues(worms.printRandomVariableValues());
        
        //baseline prevalence
        PrevalenceEvent pe = PrevalenceEvent(popln.getMinAgePrev(), scenarios.getExtraMinAge(), scenarios.getExtraMaxAge(), scenarios.getOutputtMethod());  //default age range and method to output at end of burn in
        burnIn(popln, vectors, worms, currentOutput, &pe);                    //should be at least 100 years
        //Run each scenario
        for (unsigned s = 0; s < scenarios.getNumScenarios(); s++){

            Scenario& sc = scenarios[s];
            
            if(_DEBUG) std::cout << std::endl << sc.getName() << " starts month " << sc.getStartMonth() << std::endl;
            
            if(sc.getStartMonth() != currentMonth){
                
                //reset to the start of this month
                currentMonth = sc.getStartMonth();
                //reset to the start of currentMonth
                popln.resetToMonth(currentMonth);    //worms and aImp
                vectors.resetToMonth(currentMonth);  //L3
                
                //delete any results with a month >= to this month
                currentOutput.resetToMonth(currentMonth); //MDA and prev
            }
          
            //evolve, saving any specified months along the way
            for (int y = 0; y < sc.getNumMonthsToSave(); y++)
                evolveAndSave(y, popln, vectors, worms, sc, currentOutput, rep, k_vals, v_to_h_vals, popln.getUpdateParams(), opDir);
            
            //done for this scenario, save the prevalence values for this replicate
            if(!_DEBUG) sc.printResults(rep, currentOutput, popln);
            
            if(_DEBUG) popln.printMDAHistory();

        }//end of each scenario
        if(!_DEBUG){
            std::cout << "\b\b\b\b";
            std::cout << std::setw(3) << int(rep*100/replicates) << "%";
        }
    } // end for each rep
    
    scenarios.closeFiles();
    //finished
    
}


void Model::burnIn(Population& popln, Vector& vectors, const Worm& worms, Output& currentOutput, PrevalenceEvent* pe){
    
    //burn in period. Don't need to worry about drugs
    //just save final state
    
    int steps = 12 * std::max(100, popln.getMaxAge())/dt; //one step is 1 * dt months, run for 100 years, Must be at least maxAge
    
    for(int i = 0; i < steps; i++){
        
        //updates number of worms in each hosts and increments host age
        popln.evolve(dt, vectors, worms);
        
        //update larval density in the vector population according to new mf levels in host polution
        vectors.updateL3Density(popln, worms);
            
        
    }
    
     //these are initial conditions for start of month zero
    popln.saveCurrentState(0, "burn-in"); //worms and importation rate. Scenario name just needed for debugging
    vectors.saveCurrentState(0); //larval density
    
    RecordedPrevalence prevalence = popln.getPrevalence(pe); //prev measured before mda done to kill mf in hosts
    currentOutput.saveMonth(-1, popln, pe, prevalence);
    


}


void Model::evolveAndSave(int y, Population& popln, Vector& vectors, Worm& worms, Scenario& sc, Output& currentOutput, int rep,
std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, int updateParams, std::string opDir){
    
    //advance to the next target month
    std::string folderName = opDir + "/endgame";
    int paramIndex = 0;
    int targetMonth = sc.getMonthToSave(y); //simulate to start of this month
    double mfprev = 1; //variable to check prevalence of mf for survey 
    //double icprev = 1; //variable to check prevalence of ic for survey 
    double nextSurveyTime = -100000; // variable updated to 3 years after survey if mf prevalence below threshold, don't do mda if time is before this value
    popln.totMDAs = 0;
    popln.post2020MDAs = 0;
    popln.numPreTASSurveys = 0;
    popln.numTASSurveys = 0;
    popln.t_TAS_Pass = -1;
    int MFlowNoMDA = 0;
    double mfprev_aimp_old = popln.getMFPrev(); 
    double mfprev_aimp_new = 0;
    bool preTAS_Pass = 0;
    int changeSensSpec = 0;
    int changeNeverTreat = 0;
   // int maxAge = popln.getMaxAge();
    int TAS_Pass = 0;
    int neededTASPass = 3;
    // int outputTime = floor(currentMonth/12);
    //int outputTime = 0;
    int LymphodemaTotalWorms = popln.getLymphodemaTotalWorms();
    int HydroceleTotalWorms = popln.getHydroceleTotalWorms();
    double LymphodemaShape = popln.getLymphodemaShape();
    double HydroceleShape = popln.getHydroceleShape();
    sc.InitIHMEData(rep, folderName);
    sc.InitIPMData(rep, folderName);
    int vec_control = 0;
    double prevCov = -1;
    double prevRho = -1;
    for(int q = 0; q < popln.sensSpecChangeCount; q++)  {
        if(popln.sensSpecChangeName[q] == sc.getName()){
            changeSensSpec = 1;
        }
    }
    for(int q = 0; q < popln.neverTreatChangeCount; q++)  {
        if(popln.neverTreatChangeName[q] == sc.getName()){
            changeNeverTreat = 1;
        }
        
    }


    for (int t = currentMonth; t < targetMonth; t += dt){
        paramIndex = t / 12;
        // if we are updating the k and v_to_h params, then do so if the time is right to do so
        if ((updateParams) && (t%12 == 0) && (paramIndex <= (k_vals.size()-1))){
            popln.updateKVal(k_vals[paramIndex]);
            vectors.updateVtoH(v_to_h_vals[paramIndex]);
        }

        PrevalenceEvent* outputPrev = sc.prevalenceDue(t); //defines min age of host to include and method ic/mf
        MDAEvent* applyMDA = sc.treatmentDue(t);
        if (outputPrev || applyMDA ){
            double MFPrev = popln.getMFPrev();
            sc.writePrevByAge(popln, t, rep, folderName);
            sc.writeNumberByAge(popln, t, rep, folderName);
            sc.writeSequelaeByAge(popln, t, LymphodemaTotalWorms,  LymphodemaShape, HydroceleTotalWorms, HydroceleShape, rep, folderName);
            sc.writeSurveyByAge(popln, t, preTAS_Pass, TAS_Pass, rep, folderName);
            sc.writeL3(vectors, t, preTAS_Pass, TAS_Pass,rep, folderName);
            sc.writeMF(MFPrev, t,rep, folderName);
        }
        sc.updateImportationRate(popln, t);
        sc.updateBedNetCoverage(popln, t);
       
        //updates number of worms in each hosts and increments host age
        popln.evolve(dt, vectors, worms); //simulates to the end of month t

        //update larval density in the vector population according to new mf levels in host polution
        vectors.updateL3Density(popln, worms);
        
        
        
       
        RecordedPrevalence prevalence;
       
        if(outputPrev)  //use alternative for of prevalence function as its required to return 2 values, but these must be calculated at the same time
           prevalence = popln.getPrevalence(outputPrev); //prev measured before mda done to kill mf in hosts
        
        // snippet to perform a survey
        if(t == nextSurveyTime){
            if((preTAS_Pass == 0) || (TAS_Pass < neededTASPass )){ // if we have passed pre-TAS and TAS stage, then don't do anything
                // if yet to pass pre-TAS perform pre-TAS survey
                preTAS_Pass = popln.PreTASSurvey();
                if(preTAS_Pass == 1){ 
                    // if we have passed the pre-TAS then do TAS 
                    TAS_Pass = popln.TASSurvey(t);
                }
                if(TAS_Pass == 1){
                    if(vec_control == 0){
                        vec_control = 1;
                    }
                }
                nextSurveyTime = t + popln.interSurveyPeriod;    
            }
 
        }

        //std::cout << applyMDA->getMonth() << std::endl;
        if(applyMDA){
            double cov = applyMDA->getCoverage();
            double rho = applyMDA->getCompliance();
            // if we this is the first MDA, then we need to initialise the Probability of treatment for each person
            if(prevCov == -1){
                popln.initPTreat(cov, rho);
                prevCov = cov;
                prevRho = rho;
            }
            // if the MDA parameters have changed then we need to update the probability of treatment for each person
            if((prevCov != applyMDA->getCoverage()) || (prevRho !=applyMDA->getCompliance())){
                popln.checkForZeroPTreat(prevCov, prevRho);
                popln.editPTreat(cov, rho);
                prevCov = cov;
                prevRho = rho;
            }
            // check for anyone with 0 probability of treatment, as these will be people who have not had this value initialised
            popln.checkForZeroPTreat(cov, rho);

            // if this this the first MDA then if the NoMDALowMF indicator is 1 then we need to check the MF prevalence in the population
            // as if this is low then we wil not begin MDA. If the indicator is not 1 then we will do MDA even with low MF prevalence
            if(popln.totMDAs == 0){
                if(popln.getNoMDALowMF() == 1){
                    mfprev = popln.getMFPrev(); 
                    if ((mfprev > popln.MFThreshold) && (MFlowNoMDA == 0)){
                        popln.ApplyTreatmentUpdated(applyMDA, worms, sc, t, rep, folderName);
                        popln.totMDAs += 1;
                    }else{
                        MFlowNoMDA += 1;
                        if(MFlowNoMDA == popln.firstTASNumMDA){
                         nextSurveyTime = t + 6;
                        }
                        sc.writeMDADataMissedYears(t, 0, 0, popln.getMinAgeMDA(), 100, rep,  folderName);
                    }
                }else{
                    popln.ApplyTreatmentUpdated(applyMDA, worms, sc, t, rep, folderName);
                    popln.totMDAs += 1;
                }  
             }else if(popln.totMDAs < popln.firstTASNumMDA){
                //do mda
                popln.ApplyTreatmentUpdated(applyMDA, worms, sc, t, rep, folderName);
                popln.totMDAs += 1;
                if(t >= 240){
                    popln.post2020MDAs += 1;
                }
                mfprev_aimp_new = popln.getMFPrev(); 
                if (mfprev_aimp_new < mfprev_aimp_old){
                    popln.aImp = popln.aImp * mfprev_aimp_new / mfprev_aimp_old;
                    mfprev_aimp_old = popln.getMFPrev(); 
                }
                if (popln.totMDAs == popln.firstTASNumMDA ){
                    nextSurveyTime = t + 6;
                }
            }else if(popln.totMDAs >= popln.firstTASNumMDA){
                if((preTAS_Pass == 1) && (TAS_Pass >= 1)){
                    sc.writeMDADataMissedYears(t, 0, 0, popln.getMinAgeMDA(), 100, rep,  folderName);
                }else{
                    popln.ApplyTreatmentUpdated(applyMDA, worms, sc, t, rep, folderName); //treated set. alters M, WM, WF, using covMDA set above
                    popln.totMDAs += 1;
                    if(t >= 240){
                        popln.post2020MDAs += 1;
                    }  
                    mfprev_aimp_new = popln.getMFPrev(); 
                    if (mfprev_aimp_new < mfprev_aimp_old){
                        popln.aImp = popln.aImp * mfprev_aimp_new / mfprev_aimp_old;
                        mfprev_aimp_old = popln.getMFPrev();
                    } 
                }
            }
        }
        if(t < popln.getNeverTreatChangeTime()){
            popln.neverTreatToOriginal();
        }

        if (t >= popln.getNeverTreatChangeTime()){
            if(changeNeverTreat == 1){
                popln.changeNeverTreat();
              //  std::cout << sc.getName()<< " change never treat " << std::endl;
            }
        }

        if(t < popln.getICTestChangeTime()){
            popln.ICTestToOriginal();
        }

        if (t >= popln.getICTestChangeTime()){
            if(changeSensSpec == 1){
                popln.changeICTest();
               // std::cout << sc.getName()<< " change sens spec" << std::endl;
            }
            
        }
        
        if (outputPrev || applyMDA){
            //std::cout << t << "," << popln.getMFPrev() << std::endl;
            currentOutput.saveMonth(t, popln, outputPrev, prevalence, applyMDA);
        }
        
            
        if(_DEBUG && applyMDA) std::cout << applyMDA->getType() << " month " << currentMonth  << ", MDA at " << applyMDA->getCoverage() << " coverage" << std::endl;


    
        //next t
    }
    popln.neverTreatToOriginal();
    //done
    currentMonth = targetMonth;

    // double prev1 = popln.getMFPrev();
    popln.saveTotalWorms();
    popln.saveTotalWormYears();
    popln.saveAges();
    if (y < (sc.getNumMonthsToSave()-1)){ //not finished this scenario
        popln.saveCurrentState(currentMonth, sc.getName()); //worms and importation rate. Scenario name just needed for debugging
        vectors.saveCurrentState(currentMonth); //larval density
        
        if(_DEBUG) std::cout << sc.getName() << " saving month " << currentMonth << std::endl;

    }
    
}


void Model::getRandomParameters(int index, std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, std::vector<double>& aImp_vals, std::vector<double>& wProp_vals, unsigned replicates, std::string fname){
    
    
    std::ifstream infile(fname, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Model::runScenarios. Cannot read file " << fname << std::endl;
        exit(1);
    }

    std::string line;
    
    while (getline(infile, line)){
       
        std::istringstream linestream(line);
        double k = -1.0, v_to_h = -1.0, aImp = -1.0, wProp = -1.0;
        
        linestream >> v_to_h >> k >> aImp >> wProp;
        
        if (k < 0 || v_to_h < 0 || aImp < 0){
            std::cout << "Error in Model::runScenarios. Error reading file " << fname << std::endl;
            exit(1);
            
        }
       
        k_vals.push_back(k);
        aImp_vals.push_back(aImp);
        v_to_h_vals.push_back(v_to_h);
        wProp_vals.push_back(wProp);
       
    }

    
    infile.close();
    
    if((k_vals.size() < replicates) || (v_to_h_vals.size() < replicates) || (aImp_vals.size() < replicates) || (wProp_vals.size() < replicates)){
        std::cout << "Error in Model::runScenarios. " << fname << " is too short for " << replicates << "replicates" << std::endl;
        exit(1);
        
    }
    
    
}




void Model::ProcessLine(const std::string &line, std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, std::vector<double>& aImp_vals, std::vector<double>& wProp_vals){
    std::istringstream iss(line);
    double value;
    int arraySize = k_vals.size();
    int k = 0;
    for (int j = 1; iss >> value; ++j) {
        
        if (j % 4 == 1) {
            if(arraySize >0){
                v_to_h_vals[k] = value;
            }else{
                v_to_h_vals.push_back(value);
            }
        } else if (j % 4 == 2) {
            if(arraySize >0){
                k_vals[k] = value;
            }else{
                k_vals.push_back(value);
            }
        } else if (j % 4 == 3) {
            if(arraySize >0){
                aImp_vals[k] = value;
            }else{
                aImp_vals.push_back(value);
            }
        } else if (j % 4 == 0) {
            if(arraySize >0){
                wProp_vals[k] = value;
                k++;
            }else{
                wProp_vals.push_back(value);
            }
        }
    }
    if(k != arraySize){
        std::cout << " Error" << std::endl;
        std::cerr << "number of input parameters has changed" << std::endl;
        //exit(1);
    }
}



void Model::getRandomParametersMultiplePerLine(int index, std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, std::vector<double>& aImp_vals, std::vector<double>& wProp_vals, unsigned replicates, std::string fname) {
   
     // Replace with the desired line number

    std::ifstream file(fname);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fname << std::endl;
        
    }

    for (int i = 1; i <= index; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Error reading line " << index << " from file: " << fname << std::endl;
            
        }
    }

    ProcessLine(line, k_vals, v_to_h_vals, aImp_vals, wProp_vals);
}
