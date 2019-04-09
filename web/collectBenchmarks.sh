#! /bin/bash

###########
## Input ##
###########

dir=${1:-"benchmarks"}
suite=${2:-"forPR"} # which set of benchmarks to run: full, forPR, forConf

###################
## Configuration ##
###################
source xeon_scripts/common-variables.sh ${suite}
source xeon_scripts/init-env.sh

######################################
## Move Compute Performance Results ##
######################################

# Move subroutine build benchmarks
builddir="Benchmarks"
mkdir -p ${dir}/${builddir}
mkdir -p ${dir}/${builddir}/logx

for ben_arch in SKL-SP
do
    for benchmark in TH
    do
	oBase=${ben_arch}_${sample}_${benchmark}

	mv ${oBase}_"time".png ${dir}/${builddir}
	mv ${oBase}_"speedup".png ${dir}/${builddir}

	mv ${oBase}_"time_logx".png ${dir}/${builddir}/logx
	mv ${oBase}_"speedup_logx".png ${dir}/${builddir}/logx
    done
done

# Move multiple events in flight plots
meifdir="MultEvInFlight"
mkdir -p ${dir}/${meifdir}
mkdir -p ${dir}/${meifdir}/logx

for ben_arch in SKL-SP
do
    for build in "${meif_builds[@]}"
    do echo ${!build} | while read -r bN bO
	do
            oBase=${ben_arch}_${sample}_${bN}_"MEIF"
	    
            mv ${oBase}_"time".png ${dir}/${meifdir}
            mv ${oBase}_"speedup".png ${dir}/${meifdir}
	    
            mv ${oBase}_"time_logx".png ${dir}/${meifdir}/logx
            mv ${oBase}_"speedup_logx".png ${dir}/${meifdir}/logx
	done
    done
done
