#! /bin/bash

########################
## Command Line Input ##
########################

remote_arch=${1} # SNB, KNL, SKL-SP
suite=${2:-"forPR"} # which set of benchmarks to run: full, forPR, forConf

###################
## Configuration ##
###################

source xeon_scripts/common-variables.sh ${suite}
source xeon_scripts/init-env.sh

# architecture dependent settings
if [[ "${remote_arch}" == "SNB" ]]
then
    HOST=${SNB_HOST}
    DIR=${SNB_WORKDIR}/${SNB_TEMPDIR}
elif [[ "${remote_arch}" == "KNL" ]]
then
    HOST=${KNL_HOST}
    DIR=${KNL_WORKDIR}/${KNL_TEMPDIR}
else 
    echo ${remote_arch} "is not a valid architecture! Exiting..."
    exit
fi

###################
## Run The Tests ##
###################

# execute tests remotely
echo "Executing ${remote_arch} tests remotely..."
SSHO ${HOST} bash -c "'
cd ${DIR}
./xeon_scripts/benchmark-cmssw-ttbar-fulldet-build.sh ${remote_arch} ${suite}
exit
'"

# copy logs back for plotting
echo "Copying logs back from ${remote_arch} for plotting"
scp ${HOST}:${DIR}/log_${remote_arch}_${sample}_*.txt .

# destroy tmp files
echo "Removing tmp dir on ${remote_arch} remotely"
SSHO ${HOST} bash -c "'
rm -rf ${DIR}
exit
'"
