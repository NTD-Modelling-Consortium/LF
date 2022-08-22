//
//  BedNetEvent.hpp
//  transfil
//
//  Created by Paul Brown on 23/05/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#ifndef BedNetEvent_h
#define BedNetEvent_h

class BedNetEvent {
    
public:

  
    
    BedNetEvent(int m, double c, double s): startMonth(m), coverage(c), sysComp(s) {};
    int getMonth() const;
    double getCoverage() const;
    double getSysComp() const;
    double getSigma2() const;
    
    static bool sort(const BedNetEvent& lhs, const BedNetEvent& rhs);
    
private:
    
    int startMonth;
    double coverage;
    double sysComp;
    
    
};


#endif /* BedNetEvent_h */
