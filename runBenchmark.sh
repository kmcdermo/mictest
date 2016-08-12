#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

./benchmark-cmssw-endcap.sh
python makeBenchmarkPlots.py host_endcap cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1,1\)
