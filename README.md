# LF

Along with files included in the C++ code folder, the gsl library is used so will need to be installed

**Code to compile the files**:

g++ -g -I. -I./tinyxml -I/usr/include -I/Users/matthewgraham/gsl/include -L/Users/matthewgraham/gsl/lib -Wall -O3 -std=c++11 -lm -lgsl -lgslcblas *.cpp tinyxml/*.cpp -o transfil_N

where the gsl calls will need to be changed to where gsl library is installed on your computer.

**Code to run the simulations**:

./transfil_N -s testscenario.xml -n Pop_Distribution.csv -p RandomParamIU4629.txt -r 200 -t 1 -o results/r2


