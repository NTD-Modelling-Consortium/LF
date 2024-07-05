#!/usr/bin/env bash

# bail out immediately if anything errors
set -euo pipefail

function info () {
	# shellcheck disable=SC2086,SC2027
    echo -e '\E[37;44m'"\033[1m-> "$1"\033[0m"
    tput -T linux sgr0
}

function error () {
	# shellcheck disable=SC2086,SC2027
    echo -e '\E[37;31m'"\033[1m-> "$1"\033[0m"
    tput -T linux sgr0
}

function usage () {
	error "usage: ${0} [-n <num-simulations>] [-f <running-id-list-file>] [-o <output-subdirectory>] [-j <num-parallel-jobs>] [-s <scenarios- dir>] [-p <parameters-dir>] [-r <results-dir>] [-O <output-dir>]"
	exit 1
}

# convert $1 (relative filename) to absolute
function get_abs_filename() {
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

# set up sensible defaults
NUM_SIMULATIONS=200
NUM_PARALLEL_JOBS=default
RUNNING_ID_LIST_FILE=$( get_abs_filename run/running-id-list.txt )
OUTPUT_SUBDIRECTORY=$( date +%Y%ma )
PROJECT_ROOT_DIR=$( get_abs_filename . )

# default options, potentially overridden in env
PARAMETER_ROOT="${PARAMETER_ROOT:=$( realpath ./run/parameters )}"
SCENARIO_ROOT="${SCENARIO_ROOT:=$( realpath ./run/scenarios )}"
RESULTS_ROOT="${RESULTS_ROOT:=$( realpath ./run/results )}"
OUTPUT_ROOT="${OUTPUT_ROOT:=$( realpath ./run/output/ntd )}"
STARTING_YEAR="${STARTING_YEAR:=2020}"

# read CLI options
while getopts "n:f:o:j:s:p:r:O:Y:" opts ; do

	case "${opts}" in

		n)
			NUM_SIMULATIONS=${OPTARG}
			;;

		f)
			RUNNING_ID_LIST_FILE=$( get_abs_filename "${OPTARG}" )
			if [[ ! -f "${RUNNING_ID_LIST_FILE}" || ! -s "${RUNNING_ID_LIST_FILE}" ]] ; then
				error "running ID list file must be a real path to a non-empty file"
				usage
			fi
			;;

		o)
			OUTPUT_SUBDIRECTORY=${OPTARG}
			;;

		j)
			NUM_PARALLEL_JOBS=${OPTARG}
			;;

		s)
			SCENARIO_ROOT=$( realpath "${OPTARG}" )
			;;

		p)
			PARAMETER_ROOT=$( realpath "${OPTARG}" )
			;;

		r)
			RESULTS_ROOT=$( realpath "${OPTARG}" )
			;;

		O)
			OUTPUT_ROOT=$( realpath "${OPTARG}" )
			;;

		Y)
			STARTING_YEAR=${OPTARG}
			;;

		*)
			usage
			;;

	esac

done

# check input/output directories
if [[ ! -d "${SCENARIO_ROOT}" ]] ; then
	error "scenario-dir must be a real path to a directory"
	usage
fi

if [[ ! -d "${PARAMETER_ROOT}" ]] ; then
	error "parameter-dir must be a real path to a directory"
	usage
fi

if [[ ! -d "${RESULTS_ROOT}" ]] ; then
	error "result-dir must be a real path to a directory"
	usage
fi

if [[ ! -d "${OUTPUT_ROOT}" ]] ; then
	error "output-dir must be a real path to a directory"
	usage
fi

# display info splash
NUM_IUS=$( wc -l ${RUNNING_ID_LIST_FILE} | awk '{print $1}' )
info "about to run model with these settings:"
echo "- run ${NUM_SIMULATIONS} simulations for each IU"
echo "- across ${NUM_PARALLEL_JOBS} parallel jobs"
echo "- use ID list file ${RUNNING_ID_LIST_FILE} (${NUM_IUS} IUs)"
echo "- use scenarios in directory ${SCENARIO_ROOT}"
echo "- use parameters in directory ${PARAMETER_ROOT}"
echo "- start from year ${STARTING_YEAR}"
echo "- write intermediate results in directory ${RESULTS_ROOT}"
echo "- write combined model output files to directory ${OUTPUT_ROOT}/${OUTPUT_SUBDIRECTORY}"

# confirm go-ahead
info "confirm? (enter 1 to go ahead)"
select CHOICE in yes no ; do

	case "${CHOICE}" in

		yes)

			# create a log file namae
			RUN_STAMP=$( date +%Y%m%d%H%M%S )
			LOG_FILE="lf-run-${RUN_STAMP}-output.txt"
			FINISH_FILE="lf-run-${RUN_STAMP}-finished.txt"

			# work from the 'run' directory where the Python etc lives
			cd run

			# run the parallel job in a detached process
			nohup \
				env \
					NUM_PARALLEL_JOBS="${NUM_PARALLEL_JOBS}" \
					NUM_SIMULATIONS="${NUM_SIMULATIONS}" \
					RUNNING_ID_LIST_FILE="${RUNNING_ID_LIST_FILE}" \
					RUN_STAMP="${RUN_STAMP}" \
					SCENARIO_ROOT="${SCENARIO_ROOT}" \
					PARAMETER_ROOT="${PARAMETER_ROOT}" \
					RESULTS_ROOT="${RESULTS_ROOT}" \
					OUTPUT_ROOT="${OUTPUT_ROOT}" \
                    STARTING_YEAR="${STARTING_YEAR}" \
				bash run-in-parallel.bash "${OUTPUT_ROOT}/${OUTPUT_SUBDIRECTORY}" \
					2>&1 \
					> "${PROJECT_ROOT_DIR}/${LOG_FILE}" \
					< /dev/null \
					&

			# inform user
			# shellcheck disable=SC2086
			REAL_LOG_PATH=$( realpath ${PROJECT_ROOT_DIR}/${LOG_FILE} )
			info "LF model is running in a detached shell."
			echo "Log output is being saved to file: ${REAL_LOG_PATH}"
			echo "When the model runs have finished, the file $( realpath ${PROJECT_ROOT_DIR} )/${FINISH_FILE} will be created."

			exit 0
			;;

		*)
			exit 1
			;;
	esac

done
