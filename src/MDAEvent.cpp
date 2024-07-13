//
//  MDAEvent.cpp
//  transfil
//
//  Created by Paul Brown on 12/05/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "MDAEvent.hpp"

double MDAEvent::getCoverage() const { return covMDA; }

std::string MDAEvent::getType() const { return type; }

double MDAEvent::getCompliance() const { return sysComp; }

double MDAEvent::getSigma2() const { return sysComp / (1 - sysComp); }

int MDAEvent::getMonth() const { return month; }

int MDAEvent::getMinAge() const { return minAge; }

bool MDAEvent::sort(const MDAEvent &lhs, const MDAEvent &rhs) {

  return (lhs.getMonth() < rhs.getMonth());
}
