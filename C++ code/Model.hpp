//
//  Model.hpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp


#include <vector>
#include <fstream>
#include <random>

#include "Output.hpp"


class Scenario;
class ScenariosList;
class Population;
class Vector;
class Worm;
class PrevalenceEvent;


class Model {
    
    
public:

    void runScenarios( ScenariosList& scenarios, Population& popln, Vector& vectors, Worm& worms, int replicates, double timestep, int index, std::string randParamsfile);
   
protected:
    
    void burnIn(Population& popln, Vector& vectors, const Worm& worms, Output& currentOutput, PrevalenceEvent* pe);
    void evolveAndSave(int y, Population& popln, Vector& vectors, Worm& worms, Scenario& sc, Output& currentOutput);
    void getRandomParameters(int index, std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, std::vector<double>& aImp_vals, std::vector<double>& wPropMDA, unsigned replicates, std::string fname);
    
    int currentMonth;
    double dt;
    

    
};


#endif /* Model_hpp */