//
//  BedNetEvent.cpp
//  transfil
//
//  Created by Paul Brown on 23/05/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#include "BedNetEvent.hpp"


int BedNetEvent::getMonth() const {
    
    return startMonth;
    
}

double BedNetEvent::getCoverage() const {
    
    return coverage;
    
}

double BedNetEvent::getSysComp() const {
    
    return sysComp;
    
}

double BedNetEvent::getSigma2() const {
    
    return sysComp/(1-sysComp);
    
}

bool BedNetEvent::sort(const BedNetEvent& lhs, const BedNetEvent& rhs) {
    
    return (lhs.getMonth() < rhs.getMonth());
    
}
