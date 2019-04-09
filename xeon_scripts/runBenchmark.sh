#! /bin/bash

##### Command Line Input #####
suite=${1:-"forPR"} # which set of benchmarks to run: full, forPR, forConf

##### Initialize Benchmarks #####
source xeon_scripts/common-variables.sh ${suite}
source xeon_scripts/init-env.sh
make distclean

echo "Run benchmarking on SKL-SP"
./xeon_scripts/benchmark-cmssw-ttbar-fulldet-build.sh SKL-SP ${suite}

##### Benchmark Plots #####
echo "Producing benchmarking plots"
./plotting/benchmarkPlots.sh ${suite}

##### Final cleanup #####
make distclean

##### Final message #####
echo "Finished benchmarking and validation suite!"
