#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

./benchmark-cmssw.sh
python makeBenchmarkPlots.py host cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1\)

./benchmark-cmssw-mic.sh
python makeBenchmarkPlots.py mic cmssw
root -b -q -l makeBenchmarkPlots.C\(1,1\)

for test in BH CE CEST ST TBBST; do
    python makePlotsFromDump.py _host_100xTTbarPU35_${test}_NVU1_NTH1
    python makePlotsFromDump.py _host_100xTTbarPU35_${test}_NVU8int_NTH21
    python makePlotsFromDump.py _mic_100xTTbarPU35_${test}_NVU1_NTH1
    python makePlotsFromDump.py _mic_100xTTbarPU35_${test}_NVU16int_NTH210
    root -b -q -l makePlotsFromDump.C\(\"${test}\",1\)
done
