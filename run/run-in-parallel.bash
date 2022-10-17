#!/usr/bin/env bash

if [[ $# != 1 ]] ; then
	echo "usage: ${0} <output-folder-name>" >&2
	exit 1
fi

if [[ -z "$( which parallel )" ]] ; then
	echo "=> error: please ensure you've installed GNU 'parallel' before running this script." >&2
	exit 1
fi

if [[ -z "$( which pipenv )" ]] ; then
	echo "=> error: please ensure you've installed 'pipenv' before running this script." >&2
	exit 1
fi

# destination below 'output/ntd' where the outputs should be saved, e.g. 202208b
output_folder_name="${1}"
echo "-> saving output into ./output/ntd/${1}" >&2

# work out how many jobs to run in parallel
NUM_JOBS=${NUM_JOBS:=default}
if [[ "${NUM_JOBS}" = "default" ]] ; then
	JOBS_ARG=""
else
	JOBS_ARG="-j${NUM_JOBS}"
fi

RUNNING_ID_LIST_FILE=${RUNNING_ID_LIST_FILE:=running-id-list.txt}

# run the job
NUM_SIMULATIONS=${NUM_SIMULATIONS:=5} time parallel ${JOBS_ARG} -a "${RUNNING_ID_LIST_FILE}" pipenv run bash run-lf-model.bash "${output_folder_name}"
