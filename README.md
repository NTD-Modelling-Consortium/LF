# LF

Along with files included in the `src` folder, the gsl library is used so will need to be installed. The installation process for gsl is described [here](https://coral.ise.lehigh.edu/jild13/2016/07/11/hello/).

- Alternatively on macOS you can install gsl using homebrew: `brew install gsl`
- Or on Debian/Ubuntu Linux you can install packages `gsl-bin libgsl-dev` using apt/aptitude

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
	-Wall -O3 -std=c++17
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
	-Wall -O3 -std=c++17
```

The gsl calls will need to be changed to where gsl library is installed on your computer.

This compiles the source code and included libraries into the executable file `transfil_N`. This will be used to run the simulations.

### Running simulations

To run simulations:

1) create a folder to output the results. For example, `sample_results` in the root of the project.

2) then locate the three required input files: scenario, population distribution and random parameters. For testing, there are three files in the `sample_inputs` folder in the root of the project.

* scenario.xml: the file giving the scenario we want to run.

* population_distribution.csv: file describing the population distribution.

* random_parameters.txt: file with the fitted 200 parameter sets for IU 4629. Change to whichever parameter sets we want to run.

3) run the executable with the file input and other required parameters

	`./transfil\_N -s ../sample_inputs/scenario.xml -n ../sample_inputs/population_distribution.csv -p ../sample_inputs/random_parameters.txt -r 200 -t 1 -o ../sample_results`

	where the inputs are

	* -s scenario.xml: the file giving the scenario we want to run.

	* -n population_distribution.csv: file describing the population distribution.

	* -p random_parameters.txt: file with the fitted 200 parameter sets for IU 4629. Change to whichever parameter sets we want to run.

	* -r 200: number of simualtions to run. Has to be smaller than or equal to the number of parameter sets given in the parameter file.

	* -o sample_results: folder in which to save the outputs of the simulation. This folder must be created inside the `src` folder.

	* -t 1: the simulation time step, in months

#### Setting the seed for simulations

Random seeds are set per simulation through a .txt file. Each line of the text file should contain the random seed for the corresponding simulation. For example, if line 30 of the file is "123456", then for simulation number 30, the random seed is set to "123456". The number of lines must equal the input `-r` above. e.g. 200 for the command above. This file is passed in with the argument `-g`, e.g.

`./transfil\_N -s ../sample_inputs/scenario.xml -n ../sample_inputs/population_distribution.csv -p ../sample_inputs/random_parameters.txt -g ../sample_inputs/random_seeds.txt -r 200 -t 1 -o ../sample_results`

**Note**: Additional files runIU.csv, dummy_visualizations.R and vis_functions.R were previously used for post-processing of results and can be safely ignored

## Contributing

### File formatting

This repository uses clang-format 18 to format files and this is checked on each pull request. Ideally, files should be formatted before each commit.

#### Install clang-format 18

Use `pipx` to install clang-format. [Install `pipx`](https://pipx.pypa.io/latest/installation/) if you need to.

```
pipx install clang-format
```

then follow any instructions about adding it to your path. Restart your shell, then check that is version is 18 by running 
```
clang-format --version
```

You can also use other package managers for your OS, such as `brew`, `choco` or by installing `LLVM` and using the `clang-format` packaged with it, but ensure that clang-format is version 18.

#### Running clang-format

To run clang-format on a single file and update the file with changes:
```
clang-format -i path/to/file.cpp
```

To preview changes first:
```
clang-format --dry-run path/to/file.cpp
```

To run clang-format and update all cpp, h and hpp files on Mac/Linux or Windows with git bash:
```
find . -iname '*.h' -o -iname '*.cpp' -o -iname '*.hpp'| xargs clang-format -i
```

On Windows powershell (unverified)
```
$files=(git ls-files --exclude-standard); foreach ($file in $files) { if ((get-item $file).Extension -in ".cpp", ".hpp", ".h") { clang-format $file } }
```

If you have made very large changes, it is possible that clang-format may need to be run more than once.

