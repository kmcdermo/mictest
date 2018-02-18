#! /bin/bash

## Sample, KM4MT: can use 10 mu sample with dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/10muPt0p5to10HS
physics_sample=CMSSW_TTbar_PU70
dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/PU70HS/10224.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017PU_GenSimFullINPUT+DigiFullPU_2017PU+RecoFullPU_2017PU+HARVESTFullPU_2017PU/a/
file=memoryFile.fv3.clean.writeAll.recT.072617.bin

## Common setup
nevents=1 ## KM4MT: can go as high as you like -- started with one event to make it simple
nth=1
nvu=1
seeds="--cmssw-n2seeds"
arch="SNB"
bN="CE" ## KM4MT: can switch to BH
bO="ce" ## KM4MT: can switch to bh

## Make it
mOpt="-j 12"
make clean ${mOpt}
make ${mOpt} USE_INTRINSICS:=-DMPT_SIZE=${nvu} ## KM4MT: can drop the USE_INTRINSICS to do full nVU = 8 with instrinsics

## Common base executable
oBase=${arch}_${physics_sample}_${bN}
bExe="./mkFit/mkFit ${seeds} --backward-fit --input-file ${dir}/${file} --build-${bO} --num-thr ${nth} --num-events ${nevents}" ## KM4MT: used single thread to make printouts obvious

## Building-only benchmark
echo "${oBase}: Benchmark [nTH:${nth}, nVU:${nvu}]"
${bExe} >& log_${oBase}_NVU${nvu}_NTH${nth}.txt

## Final cleanup
make distclean ${mOpt}
