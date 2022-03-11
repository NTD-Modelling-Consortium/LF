//
//  MDAEvent.hpp
//  transfil
//
//  Created by Paul Brown on 12/05/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef MDAEvent_hpp
#define MDAEvent_hpp

#include <climits>
#include <string>
#include <vector>

class MDAEvent {
    
    
public:
    
 
    MDAEvent(int mn, int age, double cov, double sc, std::string t): month(mn), minAge(age), covMDA(cov), sysComp(sc), type(t) {};
    
    
    int getMonth() const;
    double getCoverage() const;
    std::string getType() const;
    double getCompliance() const;
    double getSigma2() const;
    int getMinAge() const;
    
    static bool sort(const MDAEvent& lhs, const MDAEvent& rhs);
    
    
    
private:
    
    int month;
    int minAge;
    double covMDA; //coverage 0 - 1
    double sysComp; //must be >= 0 and < 1
    std::string type;

    
};

#endif /* MDAEvent_hpp */
