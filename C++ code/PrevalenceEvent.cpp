//
//  PrevalenceEvent.cpp
//  transfil
//
//  Created by Paul Brown on 23/05/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#include <sstream>
#include "PrevalenceEvent.hpp"


int PrevalenceEvent::getMonth() const{
    
    return month;
}


int PrevalenceEvent::getMinAge() const {
    
    return minAge;
    
}


bool PrevalenceEvent::getMethod(std::string t) const {
    
    //return true if either method list is empty, ie all needed OR t is found in list
    return (!method.size()) || ( std::find(method.begin(), method.end(), t)  != method.end() );
    
}

void PrevalenceEvent::setMethod(std::string t) {
    
    //split commas separated list
    std::istringstream input(t);
    std::string m;
    while(std::getline(input, m, ','))
        method.push_back(m);
    
}

bool PrevalenceEvent::sort(const PrevalenceEvent& lhs, const PrevalenceEvent& rhs) {
    
    return (lhs.getMonth() < rhs.getMonth());
    
}


int PrevalenceEvent::getMinAgeExtra() const {
    
    return minAgeExtra;
    
}


int PrevalenceEvent::getMaxAgeExtra() const {
    
    return maxAgeExtra;
}


