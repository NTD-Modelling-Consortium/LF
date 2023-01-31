#!/bin/bash

svn co svn+ssh://panayiotatouloupou@nero.wsbc.warwick.ac.uk/home/shared/svn/transfil
cd transfil
rm transfil
module load gnu/5.4.0
g++ -I. -I./tinyxml -I/usr/include  -L/usr/lib64  -Wall -O3 -std=c++11  -lm -lgsl -lgslcblas main.cpp ScenariosList.cpp Scenario.cpp Population.cpp Host.cpp Model.cpp Output.cpp Vector.cpp Worm.cpp Statistics.cpp MDAEvent.cpp BedNetEvent.cpp PrevalenceEvent.cpp ImportationRateEvent.cpp tinyxml/tinystr.cpp tinyxml/tinyxmlparser.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxml.cpp -o transfil
