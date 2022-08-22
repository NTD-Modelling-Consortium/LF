#!/usr/bin/env bash

if [[ $# != 1 ]] ; then
	echo "usage: ${0} <output-folder-name>"
	exit 1
fi

# destination below 'combined_output/ntd' where the outputs should be saved, e.g. 202208b
output_folder_name="${1}"

NUM_SIMULATIONS=${NUM_SIMULATIONS:=5} time parallel -a running-id-list.txt bash run-lf-model.bash "${output_folder_name}"
