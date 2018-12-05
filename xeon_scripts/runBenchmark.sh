#! /bin/bash

##### Command Line Input #####
suite=${1:-"forConf"} # which set of benchmarks to run: full, forPR, forConf

##### Initialize Benchmarks #####
source xeon_scripts/common-variables.sh ${suite}
source xeon_scripts/init-env.sh
make distclean

##### Launch Tests #####
echo "Tar and send to KNL"
./xeon_scripts/tarAndSendToRemote.sh KNL ${suite}

echo "Run benchmarking on KNL concurrently with SKL-SP benchmarks" 
./xeon_scripts/benchmark-cmssw-ttbar-fulldet-build-remote.sh KNL ${suite} >& benchmark_knl_dump.txt &

echo "Run benchmarking on SKL-SP"
./xeon_scripts/benchmark-cmssw-ttbar-fulldet-build.sh SKL-SP ${suite}

echo "Waiting for KNL"
wait

##### Benchmark Plots #####
echo "Producing benchmarking plots"
./plotting/benchmarkPlots.sh ${suite}

##### Final cleanup #####
make distclean
