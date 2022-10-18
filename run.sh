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
	error "usage: ${0} [-n <num-simulations>] [-f <running-id-list-file>] [-o <output-subdirectory>] [-j <num-parallel-jobs>]"
	exit 1
}

function get_abs_filename() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

NUM_SIMULATIONS=200
NUM_PARALLEL_JOBS=default
RUNNING_ID_LIST_FILE=$( get_abs_filename run/running-id-list.txt )
OUTPUT_SUBDIRECTORY=$( date +%Y%ma )
PROJECT_ROOT_DIR=$( get_abs_filename . )

while getopts "n:f:o:j:" opts ; do

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

		*)
			usage
			;;

	esac

done

# display info splash
info "about to run model with these settings:"
echo "- run ${NUM_SIMULATIONS} simulations for each IU"
echo "- across ${NUM_PARALLEL_JOBS} parallel jobs"
echo "- use ID list file ${RUNNING_ID_LIST_FILE}"
echo "- output combined model results to directory ./run/output/ntd/${OUTPUT_SUBDIRECTORY}"

# confirm go-ahead
info "confirm? (enter 1 to go ahead)"
select CHOICE in yes no ; do

	case "${CHOICE}" in

		yes)

			# create a log file namae
			RUN_STAMP=$( date +%Y%m%d%H%M%S )
			LOG_FILE="lf-run-${RUN_STAMP}-output.txt"

			# work from the 'run' directory where the Python etc lives
			cd run

			# run the parallel job in a detached process
			nohup \
				env \
					NUM_PARALLEL_JOBS="${NUM_PARALLEL_JOBS}" \
					NUM_SIMULATIONS="${NUM_SIMULATIONS}" \
					RUNNING_ID_LIST_FILE="${RUNNING_ID_LIST_FILE}" \
					RUN_STAMP="${RUN_STAMP}" \
				bash run-in-parallel.bash "${OUTPUT_SUBDIRECTORY}" \
					2>&1 \
					> "${PROJECT_ROOT_DIR}/${LOG_FILE}" \
					< /dev/null \
					&

			# inform user
			# shellcheck disable=SC2086
			info "LF model is running in a detached shell"
			echo "log output is being saved to file: $( realpath ${PROJECT_ROOT_DIR}/${LOG_FILE} )"

			exit 0
			;;

		*)
			exit 1
			;;
	esac

done
