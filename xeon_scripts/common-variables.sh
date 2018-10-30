#! /bin/bash

# command line input
suite=${1:-"forPR"} # which set of benchmarks to run: full, forPR, forConf

# samples
export sample=CMSSW_TTbar_PU70

# Validation architecture
export val_arch=SKL-SP

# vars for KNL
export KNL_HOST=${USER}@phi2.t2.ucsd.edu
export KNL_WORKDIR=/data1/work/${USER}
export KNL_TEMPDIR=tmp

# vars for SNB
export SNB_HOST=${USER}@phi1.t2.ucsd.edu
export SNB_WORKDIR=/data2/nfsmic/${USER}
export SNB_TEMPDIR=tmp

# SSH options
function SSHO()
{
    ssh -o StrictHostKeyChecking=no < /dev/null "$@"
}
export -f SSHO

#################
## Build Types ##
#################

export BH="BH bh"
export STD="STD std"
export CE="CE ce"
export FV="FV fv"

# which set of builds to use based on input from command line
if [[ "${suite}" == "full" ]]
then
    declare -a ben_builds=(BH STD CE FV)
    declare -a val_builds=(BH STD CE FV)
elif [[ "${suite}" == "forPR" ]]
then
    declare -a ben_builds=(BH CE)
    declare -a val_builds=(STD CE)
elif [[ "${suite}" == "forConf" ]]
then
    declare -a ben_builds=(CE)
    declare -a val_builds=(CE)
else
    echo ${suite} "is not a valid benchmarking suite option! Exiting..."
    exit
fi

# set dependent arrays
th_builds=() ## for parallelization tests
vu_builds=() ## for vectorization tests
meif_builds=() ## for multiple-events-in-flight tests
text_builds=() ## for text dump comparison tests

# loop over ben_builds and set dependent arrays, export when done
for ben_build in "${ben_builds[@]}"
do
    # set th builds : all benchmarks!
    th_builds+=("${ben_build}")
    
    # set vu builds : exclude FV since it does not have a meaningful implementation outside of max VU
    if [[ "${ben_build}" != "FV" ]]
    then
	vu_builds+=("${ben_build}")
    fi
    
    # set meif builds : only do CE and FV
    if [[ "${ben_build}" == "CE" ]] || [[ "${ben_build}" == "FV" ]]
    then
	meif_builds+=("${ben_build}")
    fi
done
export ben_builds val_builds th_builds vu_builds meif_builds

# th checking
function CheckIfTH ()
{
    local build=${1}
    local result="false"

    for th_build in "${th_builds[@]}"
    do 
	if [[ "${th_build}" == "${build}" ]]
	then
	    result="true"
	    break
	fi
    done
    
    echo "${result}"
}
export -f CheckIfTH

# vu checking
function CheckIfVU ()
{
    local build=${1}
    local result="false"

    for vu_build in "${vu_builds[@]}"
    do 
	if [[ "${vu_build}" == "${build}" ]]
	then
	    result="true"
	    break
	fi
    done
    
    echo "${result}"
}
export -f CheckIfVU

# meif checking
function CheckIfMEIF ()
{
    local build=${1}
    local result="false"

    for meif_build in "${meif_builds[@]}"
    do 
	if [[ "${meif_build}" == "${build}" ]]
	then
	    result="true"
	    break
	fi
    done
    
    echo "${result}"
}
export -f CheckIfMEIF

# set text dump builds: need builds matched in both TH and VU tests
for ben_build in "${ben_builds[@]}"
do 
    check_th=$( CheckIfTH ${ben_build} )
    check_vu=$( CheckIfVU ${ben_build} )

    if [[ "${check_th}" == "true" ]] && [[ "${check_vu}" == "true" ]]
    then
	text_builds+=("${ben_build}")
    fi
done

export text_builds

# text checking
function CheckIfText ()
{
    local build=${1}
    local result="false"

    for text_build in "${text_builds[@]}"
    do 
	if [[ "${text_build}" == "${build}" ]]
	then
	    result="true"
	    break
	fi
    done

    echo "${result}"
}
export -f CheckIfText
