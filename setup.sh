#!/usr/bin/env bash

# bail out immediately if anything errors
set -euo pipefail

# check for packaged GSL install
if [[ -z $( which gsl-config ) ]] ; then
	echo "=> error: please install 'gsl' using your system package manager ('brew install gsl' / 'apt-get install -y gsl-bin libgsl-dev')." >&2
	exit 1
fi

# check for packaged pipenv install
if [[ -z $( which pipenv ) ]] ; then
	echo "=> error: please install 'pipenv' using your system package manager (homebrew/aptitude)" >&2
	exit 1
fi

# check for packaged pgrep install
if [[ -z $( which pgrep ) ]] ; then
	echo "=> error: please install 'pgrep' using your system package manager (homebrew:pgrep/aptitude:procps)" >&2
	exit 1
fi

# do the C++ build
echo "-> building C++ LF model ..."

EXENAME="transfil_N"
PROJECT_ROOT="${PWD}"
RUN_DIR="${PROJECT_ROOT}/run"
cd src
make && ( [[ -f "${EXENAME}" ]] && cp "${EXENAME}" "${RUN_DIR}/${EXENAME}" ) && echo

echo "-> checking for executable in ${RUN_DIR} ..."

cd "${PROJECT_ROOT}"
ls -l "${RUN_DIR}/${EXENAME}"
echo

# set up the python virtualenv
echo "-> setting up python LF model runner ..."

cd "${RUN_DIR}"
pipenv install --dev
pipenv run pip install .
echo

# extract data files
echo "-> extracting scenario and parameter files ..."

tar jxf parameters.tbz
tar jxf scenarios.tbz
ls -ld parameters scenarios
cd "${PROJECT_ROOT}"
cp src/Pop_Distribution.csv run
echo

echo "-> LF model and runner are built and ready to run."
