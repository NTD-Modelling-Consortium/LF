# LF

Along with files included in the `src` folder, the gsl library is used so will need to be installed. The installation process for gsl is described [here](https://coral.ise.lehigh.edu/jild13/2016/07/11/hello/).

Alternatively on macOS you can install gsl using homebrew: `brew install gsl`

### Compiling

From within `src` folder the following command should be run in the terminal in order to compile and run simulations:

```
g++ -g -I. -I./tinyxml -I/usr/include -I/Users/matthewgraham/gsl/include -L/Users/matthewgraham/gsl/lib -Wall -O3 -std=c++11 -lm -lgsl -lgslcblas \*.cpp tinyxml/\*.cpp -o transfil_N
```

On MacOS, using homebrewed `gsl`:

```
g++ \
	*.cpp tinyxml/*.cpp \
	-o transfil_N \
	-g -I. -I./tinyxml \
	$( $(brew --prefix)/bin/gsl-config --libs ) \
	$( $(brew --prefix)/bin/gsl-config --cflags ) \
	-Wall -O3 -std=c++11
```

On Debian Linux 11+ using `g++` from `build-essential` you need to specify `-lstdc++fs` in the build command:

```
g++ \
	*.cpp tinyxml/*.cpp \
	-o transfil_N \
	-g -I. -I./tinyxml \
	$(gsl-config --libs) \
	$(gsl-config --cflags) \
	-lstdc++fs \
	-Wall -O3 -std=c++11
```

The gsl calls will need to be changed to where gsl library is installed on your computer.

### Running simulations

`./transfil\_N -s testscenario.xml -n Pop_Distribution.csv -p RandomParamIU4629.txt -r 200 -t 1 -o results`

**Inputs**:

-s testscenario.xml: the file giving the scenario we want to run.

-n Pop_Distribution.csv: file describing the population distribution.

-p RandomParamIU4629.txt: file with the fitted 200 parameter sets for IU 4629. Change to whichever parameter sets we want to run.

-r 200: number of simualtions to run. Has to be smaller than or equal to the number of parameter sets given in the parameter file.

-o results: folder in which to save the outputs of the simulation. This folder must be created inside the `src` folder.



**Note**: Additional files runIU.csv, dummy_visualizations.R and vis_functions.R were previously used for post-processing of results and can be safely ignored 

