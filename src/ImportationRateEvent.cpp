//
//  ImportationRateEvent.cpp
//  transfil
//
//  Created by Paul Brown on 19/06/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#include "ImportationRateEvent.hpp"

bool ImportationRateEvent::sort(const ImportationRateEvent &lhs,
                                const ImportationRateEvent &rhs) {

  return (lhs.getMonth() < rhs.getMonth());
}

int ImportationRateEvent::getMonth() const { return month; }

double ImportationRateEvent::getFactor() const { return factor; }
