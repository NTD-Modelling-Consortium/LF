//
//  PrevalenceEvent.hpp
//  transfil
//
//  Created by Paul Brown on 23/05/2018.
//  Copyright © 2018 Paul Brown. All rights reserved.
//

#ifndef PrevalenceEvent_hpp
#define PrevalenceEvent_hpp

#include "Population.hpp"
#include <string>

class PrevalenceEvent {

  // A list of these represents all the months to outupt prevalance

public:
  PrevalenceEvent()
      : month(0), minAge(0), minAgeExtra(0),
        maxAgeExtra(Population::getMaxAge()) {};
  PrevalenceEvent(int a)
      : month(0), minAge(a), minAgeExtra(0),
        maxAgeExtra(Population::getMaxAge()) {};
  PrevalenceEvent(int a, std::string t)
      : month(0), minAge(a), minAgeExtra(0),
        maxAgeExtra(Population::getMaxAge()) {

    setMethod(t);
  };

  PrevalenceEvent(int m, int a, int extramin, int extramax)
      : month(m), minAge(a), minAgeExtra(extramin), maxAgeExtra(extramax) {};
  PrevalenceEvent(int m, int a, std::string t)
      : month(m), minAge(a), minAgeExtra(0),
        maxAgeExtra(Population::getMaxAge()) {

    setMethod(t);
  };
  PrevalenceEvent(int a, int extramin, int extramax, std::string t)
      : month(0), minAge(a), minAgeExtra(extramin), maxAgeExtra(extramax) {

    setMethod(t);
  };
  PrevalenceEvent(int m, int a, int extramin, int extramax, std::string t)
      : month(m), minAge(a), minAgeExtra(extramin), maxAgeExtra(extramax) {

    setMethod(t);
  };

  void setMethod(std::string t);
  int getMonth() const;
  int getMinAge() const;
  bool getMethod(std::string method) const;

  int getMinAgeExtra() const;
  int getMaxAgeExtra() const;
  bool requiresExtra() const;

  static bool sort(const PrevalenceEvent &lhs, const PrevalenceEvent &rhs);

private:
  int month;
  int minAge;
  std::vector<std::string> method;

  int minAgeExtra;
  int maxAgeExtra;
};

#endif /* PrevalenceEvent_hpp */
