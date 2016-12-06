#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

./benchmark.sh
python makeBenchmarkPlots.py host
root -b -q -l makeBenchmarkPlots.C

./benchmark-cmssw.sh
python makeBenchmarkPlots.py host cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1\)

./benchmark-cmssw-endcap.sh
python makeBenchmarkPlots.py host_endcap cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1,1\)

./benchmark-mic.sh
python makeBenchmarkPlots.py mic
root -b -q -l makeBenchmarkPlots.C\(1\)

./benchmark-cmssw-mic.sh
python makeBenchmarkPlots.py mic cmssw
root -b -q -l makeBenchmarkPlots.C\(1,1\)

./benchmark-cmssw-endcap-mic.sh
python makeBenchmarkPlots.py mic_endcap cmssw
root -b -q -l makeBenchmarkPlots.C\(1,1,1\)
