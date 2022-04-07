# LF

Along with files included in the C++ code folder, the gsl library is used so will need to be installed. The installation process for gsl is described here: https://coral.ise.lehigh.edu/jild13/2016/07/11/hello/

From within C++ code folder the following code should be run in the terminal in order to compile and run simulations.

**Code to compile the files**:

g++ -g -I. -I./tinyxml -I/usr/include -I/Users/matthewgraham/gsl/include -L/Users/matthewgraham/gsl/lib -Wall -O3 -std=c++11 -lm -lgsl -lgslcblas \*.cpp tinyxml/\*.cpp -o transfil_N


The gsl calls will need to be changed to where gsl library is installed on your computer.

**Code to run the simulations**:

./transfil_N -s testscenario.xml -n Pop_Distribution.csv -p RandomParamIU4629.txt -r 200 -t 1 -o results

**inputs to this**:

-s testscenario.xml: the file giving the scenario we want to run.

-n Pop_Distribution.csv: file describing the population distribution.

-p RandomParamIU4629.txt: file with the fitted 200 parameter sets for IU 4629. Change to whichever parameter sets we want to run.

-r 200: number of simualtions to run. Has to be smaller than or equal to the number of parameter sets given in the parameter file.

-o results: folder in which to save the outputs of the simulation. This folder must be created inside the C++ code folder.





