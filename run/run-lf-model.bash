#!/usr/bin/env bash
# shellcheck disable=SC2086

# default options, potentially overridden in env
NUM_SIMULATIONS=${NUM_SIMULATIONS:=5}
PARAMETER_ROOT="${PARAMETER_ROOT:=./parameters}"
SCENARIO_ROOT="${SCENARIO_ROOT:=./scenarios}"
RESULTS_ROOT="${RESULTS_ROOT:=./results}"
OUTPUT_ROOT="${OUTPUT_ROOT:=$( realpath ./run/output/ntd )}"

# run the model for a given ID, provided as $2 and passed in at bottom
function run_ID () {

	output_folder_name=${1}
	id=${2}

	PARAMS="${PARAMETER_ROOT}/${PARAMETER_FILE_STEM}${id}.txt"
	SCENARIO="${SCENARIO_ROOT}/${SCENARIO_FILE_STEM}${id}.xml"
	SEED="${SEED_ROOT}/${SEED_FILE_STEM}${id}.txt"
	RESULTS="${RESULTS_ROOT}/${id}"
	STARTING_YEAR="${STARTING_YEAR:=2020}"

	if [[ "${USE_SEED_FILE}" = "true" ]] && [[ -n $( realpath "${SEED}" 2>/dev/null ) ]] ; then
		SEED_ARG="${SEED}"
		echo "--> WARNING not using seed file ${SEED} as it doesn't exist"
	else
		SEED_ARG=1
	fi

	echo "== making result directory ${RESULTS} for ID ${id}"

	mkdir -p "${RESULTS}"

	echo "== running ${NUM_SIMULATIONS} simulations of LF model starting in ${STARTING_YEAR} with ${PARAMS} ${SCENARIO}"
	echo time ./transfil_N \
		-s "${SCENARIO}" \
		-p "${PARAMS}" \
		-t "${TIMESTEP}" \
		-e "${OUTPUT_ENDGAME}" \
		-x "${REDUCE_IMP_VIA_XML}" \
		-g "${SEED_ARG}" \
		-o "${RESULTS}" \
		-n "${POP_DISTRIBUTION_FILE}" \
		-r "${NUM_SIMULATIONS}" \
		-D "${STARTING_YEAR}"

	exit

	echo "== combining output files for IHME & NTDMC using output folder ${output_folder_name}"
	( time do_file_combinations "${id}" "${output_folder_name}" "${RESULTS}" ) 2>&1

	echo "== clearing out model 'result' files"
	rm -rf "${RESULTS}"
}

function do_file_combinations () {

	id=${1}
	output_folder_name=${2}
    result_folder_path=${3}

	for scen_iu in $( xmllint --xpath "/Model/ScenarioList/scenario/@name" <( tail -n +2 ${SCENARIO_ROOT}/scenariosNoImp${id}.xml ) | sed 's/name="\([^"]*\)"/\1/g' ) ; do

		scen=$( echo $scen_iu | cut -f 1 -d _ )
		iu=$( echo $scen_iu | cut -f 2 -d _ )

        echo "  -- running file combinations for scenario ${scen} in IU ${iu}"

		combine_output_files "${scen}" "${iu}" IHME "${output_folder_name}" "${result_folder_path}"
		combine_output_files "${scen}" "${iu}" NTDMC "${output_folder_name}" "${result_folder_path}"

	done

	echo "== done combining output files"
}

function combine_output_files () {

	scen=${1}
	iu=${2}
	inst=${3^^}
	output_folder_name=${4}
    result_folder_path=${5}

	echo "== combining ${inst} files for IU ${iu} scenario ${scen}"

	# where does the model write its output
	MODEL_OUTPUT_FILE_ROOT=${result_folder_path}/${inst}_scen${scen}/${scen}_${iu}

	# where do we want to put the combined file
	IU_OUTPUT_PATH="${output_folder_name}/lf/scenario_${scen}/${iu:0:3}/${iu}"

	# what is the file going to be called
	LOCAL_IU_OUTPUT_DIR="${IU_OUTPUT_PATH}"
	LOCAL_IU_OUTPUT_FILE_NAME="${inst,,}-${iu}-lf-scenario_${scen}-${NUM_SIMULATIONS}.csv"
	LOCAL_IU_OUTPUT_FILE_PATH="${LOCAL_IU_OUTPUT_DIR}/${LOCAL_IU_OUTPUT_FILE_NAME}"

	# create the output dir
	mkdir -p "${LOCAL_IU_OUTPUT_DIR}"

	# run the combiner
	python3 combine-lf-outputs.py "${NUM_SIMULATIONS}" "${MODEL_OUTPUT_FILE_ROOT}" "${LOCAL_IU_OUTPUT_FILE_PATH}"

	echo "== bzip2-ing output file ${LOCAL_IU_OUTPUT_FILE_PATH}"
	bzip2 --force --best "${LOCAL_IU_OUTPUT_FILE_PATH}"

}

if [[ $# != 2 ]] ; then
	echo "usage: ${0} <output-folder> <running-ID>"
	exit 1
fi

run_ID "${1}" "${2}"
