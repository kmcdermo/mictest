#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

#./benchmark.sh
#python makeBenchmarkPlots.py host
#root -b -q -l makeBenchmarkPlots.C

#./benchmark-mic.sh
python makeBenchmarkPlots.py mic
root -b -q -l makeBenchmarkPlots.C\(1\)
