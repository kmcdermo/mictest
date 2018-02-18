#! /bin/bash

## SNB or KNL
arch="SNB"

## In the case this is run separately from main script
source xeon_scripts/common_variables.sh

## Common setup

## KM4MT: can use 10 mu sample with dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/10muPt0p5to10HS
dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/PU70HS/10224.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017PU_GenSimFullINPUT+DigiFullPU_2017PU+RecoFullPU_2017PU+HARVESTFullPU_2017PU/a/
file=memoryFile.fv3.clean.writeAll.recT.072617.bin
nevents=1 ## KM4MT: can go as high as you like -- started with one event to make it simple
minth=1
minvu=1
seeds="--cmssw-n2seeds"

## Platform specific settings
mOpt="-j 12"
base=${arch}_${physics_sample}
maxth=24
maxvu=8
exe="./mkFit/mkFit ${seeds} --backward-fit --input-file ${dir}/${file}"

## VU and make
nvu=${minvu}

make clean ${mOpt}
make ${mOpt} USE_INTRINSICS:=-DMPT_SIZE=${nvu} ## KM4MT: can drop the USE_INTRINSICS to do full nVU = 8 with instrinsics

## Common base executable
bN="CE" ## KM4MT: can switch to BH
bO="ce" ## KM4MT: can switch to bh

oBase=${base}_${bN}
bExe="${exe} --build-${bO} --num-thr ${minth} --num-events ${nevents}" ## KM4MT: used single thread to make printouts obvious

## Building-only benchmark
echo "${oBase}: Benchmark [nTH:${minth}, nVU:${nvu}]"
${bExe} >& log_${oBase}_NVU${nvu}_NTH${minth}.txt

## Final cleanup
make distclean ${mOpt}
