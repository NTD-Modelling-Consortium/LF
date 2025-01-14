# LF

## Dependencies

 - [GSL](https://coral.ise.lehigh.edu/jild13/2016/07/11/hello/)
   - `brew install gsl`
   - `apt install gsl-bin libgsl-dev`
 - [CMake](https://cmake.org/download/)
   - `brew install cmake`
   - `apt install cmake`
 - [Catch](https://github.com/catchorg/Catch2) (optional - use `-DBUILD_TESTS=OFF` to disable building tests)
   - [Installation instructions](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository)

## Compiling

The project is compiled using CMake, and binaries are compiled into a separate folder `build`
```
mkdir build && cd build/
cmake ..
cmake --build .
```
The executable is `build/src/transfil_N`. This will be used to run the simulations.
On subsequent builds usually only `cmake --build .` needs to be run from the `build folder` (not `cmake ..`).

If you need to clean the CMake files (perhaps after modifying CMake or directory structure), then run `cmake --build .. --clean-first`

### Compile Options

| Option | Effect | Default |
| ------ | ------ | ------- |
| BUILD_TESTS | Compiles the unit tests | ON

To set an option, you change the initial cmake command:

```
cmake .. -DBUILD_TESTS=OFF
```

For example, to not compile the tests. 

### Debug build
If you need a build for debugging, build this into a separate folder `build_debug` and instead of the above run from the root directory
```
mkdir build_debug && cd build_debug/
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```
The executable for debugging is `build_debug/src/transfil_N`.

## Running simulations

To run simulations from the root directory:

1) create a folder to output the results. For example, `sample_results` in the root of the project.

2) then locate the three required input files: scenario, population distribution and random parameters. For testing, there are three files in the `sample_inputs` folder in the root of the project.

* scenario.xml: the file giving the scenario we want to run.

* population_distribution.csv: file describing the population distribution.

* random_parameters.txt: file with the fitted 200 parameter sets for IU 4629. Change to whichever parameter sets we want to run.

3) run the executable with the file input and other required parameters. The basic running of the model is done as follows:

	`src/transfil\_N -s sample_inputs/scenario.xml -n sample_inputs/population_distribution.csv -p sample_inputs/random_parameters.txt -r 200 -t 1 -o sample_results`

	where the inputs are

	* -s scenario.xml: the file giving the scenario we want to run.

	* -n population_distribution.csv: file describing the population distribution.

	* -p random_parameters.txt: file with the fitted 200 parameter sets for IU 4629. Change to whichever parameter sets we want to run.

	* -r 200: number of simualtions to run. Has to be smaller than or equal to the number of parameter sets given in the parameter file.

	* -o sample_results: folder in which to save the outputs of the simulation. This folder must be created inside the `src` folder.

	* -t 1: the simulation time step, in months

There are several optional inputs when running the model:

	* -x 1 or 0 (0 is default): set to 1 is we will reduce importation via the input xml file, the reduction will be replaced on map data

 	* -g seeds.csv: input a file containing integer seeds for the simulations. There must be at least as many seeds as runs to complete if any are provided.

  	* -c coverageReductionFile.csv: file which gives the proportion to which we will reduce the historical MDA coverage by, e.g. a 0.5 will half the historical MDA coverages

   	* -e 1 or 0 (0 is default): set to 1 if we want to output data for IHME.

     	* -D 2026: year from which to output IHME data from. Change to whatever year we want to do this from. Default is 2000.

      	* -m 0 : indicator for outputting NTDMC data. If this is omitted it will be output. If set to 0, this will not be done. Any other integer input will lead to it being output. 

       	* -N 2000: year from which to output NTDMC data from. Change to whatever year we want to do this from. Default is 2000


### Setting the seed for simulations

Random seeds are set per simulation through a .txt file. Each line of the text file should contain the random seed for the corresponding simulation. For example, if line 30 of the file is "123456", then for simulation number 30, the random seed is set to "123456". The number of lines must equal the input `-r` above. e.g. 200 for the command above. This file is passed in with the argument `-g`, e.g.

`src/transfil\_N -s sample_inputs/scenario.xml -n sample_inputs/population_distribution.csv -p sample_inputs/random_parameters.txt -g sample_inputs/random_seeds.txt -r 200 -t 1 -o sample_results`

**Note**: Additional files runIU.csv, dummy_visualizations.R and vis_functions.R were previously used for post-processing of results and can be safely ignored

## Running project tests

The project has tests in the directory `tests` and they are driven using `ctest` and written using the `Catch2` framework.

You need to have Catch2 installed on your system, and the instructions for doing this are found on the [Catch2 Github page](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository).

To run the tests, ensure the project is built as specified above, then run:

```bash
$ ctest --test-dir build/tests
```

This will execute the tests and give a brief report.

Note that if you've built the project into a different directory, then the folder specified in `--test-dir` will need to change. For example for the debug build as specified above it is `build_debug/tests`.

To add new tests, add the test files to `TESTS_TO_RUN` in `tests/CMakeLists.txt`.

## Contributing

### Add new cpp files

The project is build using CMake. Any new C++ files will need to be added to the sources list in `src/CMakeLists.txt`.

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
