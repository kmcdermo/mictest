#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

##### Generate toyMC samples first #####
./generateToyMCsamples.sh

##### SNB Tests #####
## ToyMC ##
./benchmark-snb-toymc-barrel-build.sh
python makeBenchmarkPlots.py snb
root -b -q -l makeBenchmarkPlots.C

## CMSSW ##
./benchmark-snb-cmssw-barrel-build.sh
python makeBenchmarkPlots.py snb cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1\)

./benchmark-snb-cmssw-endcap-build.sh
python makeBenchmarkPlots.py snb_endcap cmssw
root -b -q -l makeBenchmarkPlots.C\(0,1,1\)

##### KNC Tests #####
## ToyMC ##
./benchmark-knc-toymc-barrel-build.sh
python makeBenchmarkPlots.py knc
root -b -q -l makeBenchmarkPlots.C\(1\)

## CMSSW ##
./benchmark-knc-cmssw-barrel-build.sh
python makeBenchmarkPlots.py knc cmssw
root -b -q -l makeBenchmarkPlots.C\(1,1\)

./benchmark-knc-cmssw-endcap-build.sh
python makeBenchmarkPlots.py knc_endcap cmssw
root -b -q -l makeBenchmarkPlots.C\(1,1,1\)

make distclean
