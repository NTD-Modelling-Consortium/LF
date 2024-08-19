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
	error "usage: ${0} \\ \n
		\t[-n <num-simulations>] [-j <num-parallel-jobs>] \\ \n
		\t[-f <running-id-list-file>] [-D <pop-distribution-file>] \\ \n
		\t[-s <scenarios-dir>] [-S <scenario-file-stem(=scenariosNoImp)>]\\ \n
		\t[-p <parameters-dir>] [-P <parameter-file-stem(=RandomParamIU)>]\\ \n
		\t[-u <use-seed-file(true|=false)>] [-U <seeds-dir>] [-V <seed-file-stem(=RandomSeeds_)>]\\ \n
		\t[-r <results-dir>] [-k <keep-intermediate-results(true|=false)>] \\ \n
		\t[-O <output-dir>] [-o <output-subdir>] \\ \n
		\t[-Y <starting-year(=2020)>] [-t <timestep(=1)>] \\ \n
		\t[-x <reduce-imp-via-xml(true|=false)>] [-e <output_endgame(=true|false)>]"
	exit 1
}

# convert $1 (relative filename) to absolute
function get_abs_filename() {
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

function check_true_false () {
	case "${1}" in
		true|false)
			;;
		*)
			error "${2} must either be 'true' or 'false'"
			usage
			;;
	esac
}

function display_true_false () {
	echo -n "- "
	if [[ "${1}" = "false" ]] ; then
		echo -n "don't "
	fi
	echo "${2}"
}

# set up sensible defaults
NUM_SIMULATIONS=200
NUM_PARALLEL_JOBS=default
RUNNING_ID_LIST_FILE=$( get_abs_filename run/running-id-list.txt )
POP_DISTRIBUTION_FILE="${POP_DISTRIBUTION_FILE:=./run/Pop_Distribution.csv}"
OUTPUT_SUBDIRECTORY=$( date +%Y%ma )
PROJECT_ROOT_DIR=$( get_abs_filename . )

# default options, potentially overridden in env
PARAMETER_ROOT="${PARAMETER_ROOT:=$( realpath ./run/parameters )}"
PARAMETER_FILE_STEM="${PARAMETER_FILE_STEM:=RandomParamIU}"
SEED_ROOT="${SEED_ROOT:=$( [[ -d ./run/seeds ]] && echo $( realpath ./run/seeds ) || echo '' )}"
SEED_FILE_STEM="${SEED_FILE_STEM:=RandomSeeds_}"
SCENARIO_ROOT="${SCENARIO_ROOT:=$( realpath ./run/scenarios )}"
SCENARIO_FILE_STEM="${SCENARIO_FILE_STEM:=scenariosNoImp}"
RESULTS_ROOT="${RESULTS_ROOT:=$( realpath ./run/results )}"
OUTPUT_ROOT="${OUTPUT_ROOT:=$( realpath ./run/output/ntd )}"

USE_SEED_FILE="${USE_SEED_FILE:=false}"

STARTING_YEAR="${STARTING_YEAR:=2020}"
TIMESTEP="${TIMESTEP:=1}"
REDUCE_IMP_VIA_XML="${REDUCE_IMP_VIA_XML:=false}"

OUTPUT_ENDGAME="${OUTPUT_ENDGAME:=true}"
KEEP_INTERMEDIATE_RESULTS="${KEEP_INTERMEDIATE_RESULTS:=false}"

# read CLI options
while getopts "n:f:D:o:j:s:S:p:P:u:U:V:r:O:Y:e:t:x:k:" opts ; do

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

		D)
			POP_DISTRIBUTION_FILE=$( get_abs_filename "${OPTARG}" )
			if [[ ! -f "${POP_DISTRIBUTION_FILE}" || ! -s "${POP_DISTRIBUTION_FILE}" ]] ; then
				error "population distribution file must be a real path to a non-empty file"
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

		S)
			SCENARIO_FILE_STEM="${OPTARG}"
			;;

		p)
			PARAMETER_ROOT=$( realpath "${OPTARG}" )
			;;

		P)
			PARAMETER_FILE_STEM="${OPTARG}"
			;;

		u)
			check_true_false "${OPTARG}" use-seed-file
			USE_SEED_FILE="${OPTARG}"
			;;

		U)
			SEED_ROOT=$( realpath "${OPTARG}" )
			;;

		V)
			SEED_FILE_STEM="${OPTARG}"
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

		e)
			check_true_false "${OPTARG}" output-endgame
			OUTPUT_ENDGAME=${OPTARG}
			;;

		t)
			TIMESTEP=${OPTARG}
			;;

		x)
			check_true_false "${OPTARG}" reduce-imp-via-xml
			REDUCE_IMP_VIA_XML=${OPTARG}
			;;

		k)
			check_true_false "${OPTARG}" keep-intermediate-results
			KEEP_INTERMEDIATE_RESULTS=${OPTARG}
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

if [[ -z "$( ls -1 ${SCENARIO_ROOT}/${SCENARIO_FILE_STEM}* 2>/dev/null )" ]] ; then
	error "there aren't any scenario files in ${SCENARIO_ROOT} with filename stem ${SCENARIO_FILE_STEM}"
	usage
fi

if [[ ! -d "${PARAMETER_ROOT}" ]] ; then
	error "parameter-dir must be a real path to a directory"
	usage
fi

if [[ -z "$( ls -1 ${PARAMETER_ROOT}/${PARAMETER_FILE_STEM}* 2>/dev/null )" ]] ; then
	error "there aren't any parameter files in ${PARAMETER_ROOT} with filename stem ${PARAMETER_FILE_STEM}"
	usage
fi

if [[ -n "${SEED_ROOT}" ]] && [[ ! -d "${SEED_ROOT}" ]] ; then
	error "seed-dir must be a real path to a directory"
	usage
fi

if [[ "${USE_SEED_FILE}" = "true" ]] && [[ -z "${SEED_ROOT}" ]] ; then
	error "seed-dir must be specified if using seed files"
	usage
fi

if [[ -z "$( ls -1 ${SEED_ROOT}/${SEED_FILE_STEM}* 2>/dev/null )" ]] ; then
	error "there aren't any seed files in ${SEED_ROOT} with filename stem ${SEED_FILE_STEM}"
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

if [[ ! "${TIMESTEP}" =~ ^-?[0-9]+$ ]] ; then
	error "timestep must be an integer"
	usage
fi

# display info splash
NUM_IUS=$( wc -l ${RUNNING_ID_LIST_FILE} | awk '{print $1}' )
info "about to run model with these settings:"
echo "- run ${NUM_SIMULATIONS} simulations for each IU"
echo "- across ${NUM_PARALLEL_JOBS} parallel jobs"
echo "- use ID list file ${RUNNING_ID_LIST_FILE} (${NUM_IUS} IUs)"
echo "- use population distribution file ${POP_DISTRIBUTION_FILE}"
echo "- use scenarios in directory ${SCENARIO_ROOT}"
echo "- use scenario filename stem '${SCENARIO_FILE_STEM}'"
echo "- use parameters in directory ${PARAMETER_ROOT}"
echo "- use parameter filename stem '${PARAMETER_FILE_STEM}'"
if [[ "${USE_SEED_FILE}" = "true" ]] && [[ -n "${SEED_ROOT}" ]] ; then
	echo "- use seed files in directory ${SEED_ROOT}"
	echo "- use seed filename stem '${SEED_FILE_STEM}'"
else
	echo "- use standard seed (1)"
fi

echo "- start from year ${STARTING_YEAR}"
display_true_false "${OUTPUT_ENDGAME}" "output endgame"
display_true_false "${REDUCE_IMP_VIA_XML}" "reduce IMP via XML"
echo "- use timestep ${TIMESTEP}"

echo "- write intermediate results in directory ${RESULTS_ROOT}"
display_true_false "${KEEP_INTERMEDIATE_RESULTS}" "keep intermediate result files"
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
					SCENARIO_FILE_STEM="${SCENARIO_FILE_STEM}" \
					PARAMETER_ROOT="${PARAMETER_ROOT}" \
					PARAMETER_FILE_STEM="${PARAMETER_FILE_STEM}" \
					RESULTS_ROOT="${RESULTS_ROOT}" \
					SEED_ROOT="${SEED_ROOT}" \
					SEED_FILE_STEM="${SEED_FILE_STEM}" \
					OUTPUT_ROOT="${OUTPUT_ROOT}" \
					STARTING_YEAR="${STARTING_YEAR}" \
					USE_SEED_FILE="${USE_SEED_FILE}" \
					KEEP_INTERMEDIATE_RESULTS="${KEEP_INTERMEDIATE_RESULTS}" \
					OUTPUT_ENDGAME="${OUTPUT_ENDGAME}" \
					REDUCE_IMP_VIA_XML="${REDUCE_IMP_VIA_XML}" \
					TIMESTEP="${TIMESTEP}" \
					POP_DISTRIBUTION_FILE="${POP_DISTRIBUTION_FILE}" \
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
