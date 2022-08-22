//
//  RecordedPrevalance.hpp
//  transfil
//
//  Created by Paul Brown on 05/12/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#ifndef RecordedPrevalance_hpp
#define RecordedPrevalance_hpp

#include <vector>

//Class to record the differnt type of prevalence calculated at one output time point

class RecordedPrevalence {
    
    friend class Population;
    friend class Scenario;
    
public:
    RecordedPrevalence() : MF(0.0), IC(0.0), WC(0.0), MFRestrictedAge(0.0),  ICRestrictedAge(0.0), WCRestrictedAge(0.0) {};
    RecordedPrevalence(int size) : MF(0.0), IC(0.0), WC(0.0), MFRestrictedAge(0.0),  ICRestrictedAge(0.0), WCRestrictedAge(0.0) {
        
        hostAges.reserve(size);
        
    };
    
    RecordedPrevalence& operator=(const RecordedPrevalence&);
    
    void saveAge(int age);
    int getAgeInRange(int lower, int upper) const;
    double MF;
    
private:
    
    
    double IC;
    double WC;
    double MFRestrictedAge;
    double ICRestrictedAge;
    double WCRestrictedAge;
    std::vector<int> hostAges;
    
};

#endif /* RecordedPrevalance_hpp */
