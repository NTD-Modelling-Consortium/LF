//
//  Worm.cpp
//  transfil
//
//  Created by Paul Brown on 08/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "Worm.hpp"
#include "Statistics.hpp"
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include <fstream>
#include <iostream>

extern Statistics stats;

Worm::Worm(TiXmlElement *xmlParameters) {

  int gotAll = 0;

  TiXmlElement *xmlWorm = xmlParameters->FirstChildElement("worm");

  if (xmlWorm == NULL) {
    std::cout << "Error in Worm::Worm. Cannot find worm parameters."
              << std::endl;
    exit(1);
  }

  TiXmlElement *xmlParam = NULL;

  for (xmlParam = xmlWorm->FirstChildElement("param"); xmlParam != NULL;
       xmlParam = xmlParam->NextSiblingElement("param")) {

    std::string name;
    double value;
    name.assign(xmlParam->Attribute("name"));
    xmlParam->Attribute("value", &value);

    if (name == "nu") {
      nu = value;
      gotAll++;
    } else if (name == "alpha") {
      alpha = value;
      gotAll++;
    } else if (name == "psi1") {
      psi1 = value;
      gotAll++;
    } else if (name == "psi2") {
      psi2 = value;
      gotAll++;
    } else if (name == "s2") {
      s2 = value;
      gotAll++;
    } else if (name == "mu") {
      mu = value;
      gotAll++;
    } else if (name == "gamma") {
      gamma = value;
      gotAll++;
    } else
      std::cout << "Unknown parameter " << name << " in Worm parameter list."
                << std::endl;
  }

  if (gotAll < 7) {
    std::cout << "Error in Worm::Worm. File  does not contain all the required "
                 "parameters."
              << std::endl;
    exit(1);
  } else
    std::cout << "Worm parameters: nu=" << nu << ", alpha=" << alpha
              << ", psi1=" << psi1 << ", psi2=" << psi2 << ", s2=" << s2
              << ", mu=" << mu << ", gamma=" << gamma << std::endl
              << std::endl;
}

double Worm::proportionPerBite() const {

  // relates to the number of adult worms acquired by the host per bite
  // product of prop larave leaving vector per bite, prop of these entering host
  // and prop of these reaching adulthood Multiply this by larval density to get
  // worms per bite

  return psi1 * psi2 * s2;
}

double Worm::getDeathRate() const { return mu; }

double Worm::getMFDeathRate() const { return gamma; }

double Worm::getPropLeavingVectorPerBite() const { return psi1; }

double
Worm::repRate(unsigned monthsSinceTreated, int femaleWorms,
              int maleWorms) const { // reproductive rate of worms in host

  if (monthsSinceTreated < fecRed)
    return 0.0; // no mf births in presence of drugs

  else if (nu == 0) // polygamous so if at least one male, rate is prop to num
                    // females, otherwise zero
    return (maleWorms) ? alpha * (double)femaleWorms : 0.0;

  else // monogamous so prop to num males if fewer of these
    return alpha * std::min((double)femaleWorms, (double)maleWorms / nu);
}

void Worm::reset(double wProp) {

  // different prop killed for each replicate, or <0 to indicate fixed values
  wPropMDA = wProp;
}

int Worm::wormsTreated(int W, std::string type) {

  double propWormsKilled;

  if (type == "da") {
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.55;
    fecRed = 9;
  } else if (type == "ida") {
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.55;
    fecRed = 9; // no temporary sterilization
  } else if (type == "aa") {
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.35;
    fecRed = 0;
  } else if (type == "ia") {
    // efficacy of using IA in line with original parameters from the business
    // case work
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.35;
    fecRed = 9;
  } else if (type == "ia2") {
    // efficacy of using IA in line with more pessimistic parameters from the
    // business case work
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.32;
    fecRed = 6;
  } else if (type == "ds") {
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.59;
    fecRed = 10;
  } else if (type == "ma1") {
    // efficacy of using moxidectin in line with the latest parameters from the
    // business case work
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.82;
    fecRed = 18;
  } else if (type == "ma2") {
    propWormsKilled = (wPropMDA >= 0) ? wPropMDA : 0.9;
    fecRed = 18;
  } else {
    std::cout << " Error in Worm::wormsTreated. Unknown treatment type " << type
              << std::endl;
    exit(1);
  }

  return int((1 - propWormsKilled) * double(W));
}

double Worm::mfTreated(double M, std::string type) {

  if (type == "da") {
    mfPropMDA = 1 - 1;
    fecRed = 9;
  } else if (type == "ida") {
    mfPropMDA = 1 - 0.99;
    fecRed = 9; // no temporary sterilization
  } else if (type == "aa") {
    mfPropMDA = 1 - 0;
    fecRed = 0;
  } else if (type == "ia") {
    // efficacy of using IA in line with original parameters from the business
    // case work
    mfPropMDA = 1 - 0.99;
    fecRed = 9;
  } else if (type == "ia2") {
    // efficacy of using IA in line with more pessimistic parameters from the
    // business case work
    mfPropMDA = 1 - 0.99;
    fecRed = 6;
  } else if (type == "ds") {
    mfPropMDA = 1 - 0.86;
    fecRed = 10;
  } else if (type == "ma1") {
    // efficacy of using moxidectin in line with the latest parameters from the
    // business case work
    mfPropMDA = 1 - 1;
    fecRed = 18;
  } else if (type == "ma2") {
    mfPropMDA = 1 - 1;
    fecRed = 18;
  } else {
    std::cout << " Error in Worm::wormsTreated. Unknown treatment type " << type
              << std::endl;
    exit(1);
  }

  return M * mfPropMDA;
}

std::vector<double> Worm::printRandomVariableValues() const {

  // outputs a list of name/value pairs of any length. These are passed to
  // output object to be printed to file

  std::vector<double> values = {wPropMDA};
  return values;
}

std::vector<std::string> Worm::printRandomVariableNames() const {

  std::vector<std::string> names = {"Prop worms killed by MDA"};
  return names;
}
