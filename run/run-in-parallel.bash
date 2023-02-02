#!/usr/bin/env bash

# bail out immediately if anything errors
set -euo pipefail

# default results folder
RESULTS_ROOT="${RESULTS_ROOT:=$( realpath ./run/results )}"

# convert $1 (relative filename) to absolute
function get_abs_filename() {
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

# make sure at least output-folder is specified
if [[ $# != 1 ]] ; then
	echo "usage: ${0} <output-folder-name>" >&2
	exit 1
fi

# make sure 'parallel' program is installed
if [[ -z "$( which parallel )" ]] ; then
	echo "=> error: please ensure you've installed GNU 'parallel' before running this script." >&2
	exit 1
fi

# make sure 'pipenv' program is installed
if [[ -z "$( which pipenv )" ]] ; then
	echo "=> error: please ensure you've installed 'pipenv' before running this script." >&2
	exit 1
fi

# try to read a provided run timestamp, default to now()
RUN_STAMP=${RUN_STAMP:=$( date +%Y%m%d%H%M%S )}

# destination below ${OUTPUT_ROOT} where the outputs should be saved, e.g. 202208b
output_folder_name="${1}"

# work out how many jobs to run in parallel based on environment variable
NUM_PARALLEL_JOBS=${NUM_PARALLEL_JOBS:=default}
if [[ "${NUM_PARALLEL_JOBS}" = "default" ]] ; then
	JOBS_ARG=""
else
	JOBS_ARG="-j${NUM_PARALLEL_JOBS}"
fi

# read in the running ID list file from env, default to repo version
RUNNING_ID_LIST_FILE=${RUNNING_ID_LIST_FILE:=running-id-list.txt}

# print out run-stamp info
echo "-> running LF model at ${RUN_STAMP} using ${NUM_PARALLEL_JOBS} parallel jobs"
# shellcheck disable=SC2086
echo "-> saving output into ${output_folder_name}" >&2

# run the job in parallel
NUM_SIMULATIONS=${NUM_SIMULATIONS:=5} \
	parallel ${JOBS_ARG} -a "${RUNNING_ID_LIST_FILE}" \
	pipenv run bash run-lf-model.bash "${output_folder_name}"

# clean up
echo "== LF model run ${RUN_STAMP} removing intermediate model output files in $( get_abs_filename . )/{res_endgame,results}"
rm -rf res_endgame results/*

# indicate completion
FINISH_STAMP=$( date +%Y%m%d%H%M%S )
echo "== LF model run ${RUN_STAMP} finished at ${FINISH_STAMP}."
echo "== finished running LF model at ${FINISH_STAMP}" > "../lf-run-${RUN_STAMP}-finished.txt"
