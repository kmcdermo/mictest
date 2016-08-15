#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

./benchmark-cmssw-endcap.sh
python makeBenchmarkPlots.py host_endcap cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1,1\)

./benchmark-mic.sh
./benchmark-mic-fit.sh
python makeBenchmarkPlots.py mic
root -b -q -l makeBenchmarkPlots.C\(1\)
root -b -q -l makeBenchmarkPlotsFit.C\(1\)

./benchmark-mic-fit-endcap.sh
python makeBenchmarkPlots.py mic_endcap
root -b -q -l makeBenchmarkPlotsFit.C\(1,1\)

./benchmark-cmssw-mic.sh
python makeBenchmarkPlots.py mic cmssw
root -b -q -l makeBenchmarkPlots.C\(1,1\)

# ./benchmark-cmssw-endcap-mic.sh
# python makeBenchmarkPlots.py mic_endcap cmssw
# root -b -q -l makeBenchmarkPlots.C\(1,1,1\)

for test in BH CE CEST ST TBBST; do
    python makePlotsFromDump.py _host_10x20k_${test}_NVU1_NTH1
    python makePlotsFromDump.py _host_10x20k_${test}_NVU8int_NTH21
    python makePlotsFromDump.py _mic_10x20k_${test}_NVU1_NTH1
    python makePlotsFromDump.py _mic_10x20k_${test}_NVU16int_NTH210
    root -b -q -l makePlotsFromDump.C\(\"${test}\"\)

    python makePlotsFromDump.py _host_100xTTbarPU35_${test}_NVU1_NTH1
    python makePlotsFromDump.py _host_100xTTbarPU35_${test}_NVU8int_NTH21
    python makePlotsFromDump.py _mic_100xTTbarPU35_${test}_NVU1_NTH1
    python makePlotsFromDump.py _mic_100xTTbarPU35_${test}_NVU16int_NTH210
    root -b -q -l makePlotsFromDump.C\(\"${test}\",1\)

    python makePlotsFromDump.py _host_endcap_100xTTbarPU35_${test}_NVU1_NTH1
    python makePlotsFromDump.py _host_endcap_100xTTbarPU35_${test}_NVU8int_NTH21
    python makePlotsFromDump.py _mic_endcap_100xTTbarPU35_${test}_NVU1_NTH1
    python makePlotsFromDump.py _mic_endcap_100xTTbarPU35_${test}_NVU16int_NTH210
    root -b -q -l makePlotsFromDump.C\(\"${test}\",1\)
done

