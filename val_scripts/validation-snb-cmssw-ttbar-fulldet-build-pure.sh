#! /bin/bash

## In the case this is run separately from main script
source xeon_scripts/common_variables.sh

## Common setup
dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/PU70HS/10224.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017PU_GenSimFullINPUT+DigiFullPU_2017PU+RecoFullPU_2017PU+HARVESTFullPU_2017PU/a/
file=memoryFile.fv3.clean.writeAll.recT.072617.bin

arch=SNB
nevents=1
maxth=24
maxvu=8
exe="./mkFit/mkFit --input-file ${dir}/${file} --cmssw-pureseeds --num-thr ${maxth} --num-events ${nevents}"

## Compile once
mVal="WITH_ROOT:=yes"
make -j 12 ${mVal}

## CMSSWVAL == cmssw tracks as reference
for vV in "CMSSWVAL --cmssw-val-label"
do echo ${vV} | while read -r vN vO
    do
	for bV in "CE ce"
	do echo ${bV} | while read -r bN bO
	    do
	    	oBase=${arch}_${sample}_${bN}
		bExe="${exe} ${vO} --build-${bO}"
		
		echo "${oBase}: ${vN} [nTH:${maxth}, nVU:${maxvu}int]"
		${bExe} >& log_${oBase}_NVU${maxvu}int_NTH${maxth}_${vN}.txt
		mv valtree.root valtree_${oBase}_${vN}.root
	    done
	done
    done
done

#make distclean ${mVal}
