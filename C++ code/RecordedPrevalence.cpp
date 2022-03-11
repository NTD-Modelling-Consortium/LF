//
//  RecordedPrevalance.cpp
//  transfil
//
//  Created by Paul Brown on 05/12/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#include "RecordedPrevalence.hpp"

RecordedPrevalence& RecordedPrevalence::operator=(const RecordedPrevalence& rp){
    
    MF= rp.MF;
    IC = rp.IC;
    WC = rp.WC;
    MFRestrictedAge = rp.MFRestrictedAge;
    ICRestrictedAge = rp.ICRestrictedAge;
    WCRestrictedAge = rp.WCRestrictedAge;
 
    hostAges = rp.hostAges;
    
    return *this;
}

void RecordedPrevalence::saveAge(int age) {
    
    //age is months, but store in years
    
    hostAges.push_back(age/12);
}

int RecordedPrevalence::getAgeInRange(int lower, int upper) const {
 
    //return number of hosts in specified range at this particular time step
    int count = 0;
    for (int i = 0; i < hostAges.size(); i++)
        if (hostAges[i] >= lower) {
            if (upper < 0 || upper >= hostAges[i])
                ++count;
        }
            
    return count;
    
}
