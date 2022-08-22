//
//  ScenariosList.hpp
//  transfil
//
//  Created by Paul Brown on 28/02/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef ScenariosList_hpp
#define ScenariosList_hpp

#include <string>
#include <vector>
#include <random>

#include "Scenario.hpp"
class TiXmlElement;


//container class for a list of scenarios from one region.
//Handles loading scenario data form file such as aImp and bedNet coverage
//Also controls which months of a previous simluation a new scenario can re-use
//by telling model which months to save to return to later

class ScenariosList {
    
    
public:
    
    Scenario& operator[] (const int i)  {
        
        return scenarios[i]; //shorthand for calling getScenario(i)
    };
    
    const Scenario& operator[] (const int i) const {
        
        return scenarios[i]; //shorthand for calling getScenario(i)
    };
    
    
    void createScenarios(TiXmlElement* xmlListOfScenarios, const std::string opDir);
    void openFilesandPrintHeadings(int index, Output& results);
    void closeFiles();
    
    const Scenario& getScenario(int num) const;
    unsigned long getNumScenarios() const;
    
    std::string getName() const {
        return name;
    }
    
    int getBaseYear() const {
        return baseYear;
    }
    int getNumYears() const {
        return numYears;
    }
    
    
    std::string getOutputtMethod() const {
        
        return outputMethod;
    }
    
    int getExtraMaxAge() const {
        
        return extraMaxAge;
    }
    
    int getExtraMinAge() const {
        
        return extraMinAge;
    }
    


    

protected:
    
    std::vector<Scenario> scenarios;

    int baseYear; //Used to adjust internal indexing to real year
    int numYears;
    int numMonths;

    
    
    std::string name;
    std::string outputDir;
    std::string outputMethod; //ic or mf  or wc
    int extraMinAge;
    int extraMaxAge;
  
    
};



#endif /* ScenariosList_hpp */
