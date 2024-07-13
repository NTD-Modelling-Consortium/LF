//
//  ImportationRateEvent.hpp
//  transfil
//
//  Created by Paul Brown on 19/06/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#ifndef ImportationRateEvent_hpp
#define ImportationRateEvent_hpp

class ImportationRateEvent {

  // A list of these represents all the months to outupt prevalance

public:
  ImportationRateEvent() : month(0), factor(1.0) {};
  ImportationRateEvent(int m, double f) : month(m), factor(f) {};

  int getMonth() const;
  double getFactor() const;

  static bool sort(const ImportationRateEvent &lhs,
                   const ImportationRateEvent &rhs);

private:
  int month;
  double factor;
};

#endif /* ImportationRateEvent_hpp */
