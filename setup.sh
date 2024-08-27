#!/usr/bin/env bash

# bail out immediately if anything errors
set -euo pipefail

function info () {
	# shellcheck disable=SC2086,SC2027
    echo -e '\E[37;44m'"\033[1m"$1"\033[0m"
    tput -T linux sgr0
}

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

EXENAME="transfil_N"
PROJECT_ROOT="${PWD}"
RUN_DIR="${PROJECT_ROOT}/run"
BUILD_DIR="${PROJECT_ROOT}/build"

info "-> clearing 'build' directory"
[[ -d "${BUILD_DIR}" ]] && rm -rf "${BUILD_DIR}"

info "-> building C++ LF model ..."
mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}"
cmake .. -DBUILD_TESTS=OFF && cmake --build . --clean-first && ( [[ -f "src/${EXENAME}" ]] && cp "src/${EXENAME}" "${RUN_DIR}/${EXENAME}" ) && echo

info "-> checking for executable in ${RUN_DIR} ..."

cd "${PROJECT_ROOT}"
ls -l "${RUN_DIR}/${EXENAME}"
echo

# set up the python virtualenv
info "-> setting up python LF model runner ..."

cd "${RUN_DIR}"
# clear out existing virtualenv
set +e
pipenv --rm 2>/dev/null
set -e
rm -rf Pipfile*
pipenv install --dev
pipenv run pip install .
echo

# extract data files
info "-> extracting scenario and parameter files ..."

if [[ -d parameters ]] ; then
	rm -rf parameters
fi
tar jxf parameters.tbz

if [[ -d scenarios ]] ; then
	rm -rf scenarios
fi
tar jxf scenarios.tbz

ls -ld parameters scenarios

cd "${PROJECT_ROOT}"
echo

info "-> LF model and runner are built and ready to run."
