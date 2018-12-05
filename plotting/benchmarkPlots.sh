#! /bin/bash

## input
suite=${1:-"forConf"}

source xeon_scripts/common-variables.sh ${suite}
source xeon_scripts/init-env.sh

##### Make benchmark plots for each architecture #####
for archV in "KNL knl" "SKL-SP skl-sp"
do echo ${archV} | while read -r archN archO
    do
	for build in "${ben_builds[@]}"
	do echo ${!build} | while read -r bN bO
	    do
		# see if a test was run for this build routine
		vu_check=$( CheckIfVU ${build} )
		th_check=$( CheckIfTH ${build} )

		echo "Extract benchmarking results for" ${bN} "on" ${archN}
		python plotting/makeBenchmarkPlots.py ${archN} ${sample} ${bN} ${vu_check} ${th_check}
	    done
	done
    done
done
