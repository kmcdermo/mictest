#! /bin/bash

make -j 12 WITH_ROOT=yes

dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep
file=memoryFile.fv3.clean.writeAll.recT.072617.bin

NoPU=10024.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017_GenSimFullINPUT+DigiFull_2017+RecoFull_2017+ALCAFull_2017+HARVESTFull_2017
PU35=10224.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017PU_GenSimFullINPUT+DigiFullPU_2017PU+RecoFullPU_2017PU+HARVESTFullPU_2017PU
PU70=PU70/10224.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017PU_GenSimFullINPUT+DigiFullPU_2017PU+RecoFullPU_2017PU+HARVESTFullPU_2017PU

base=SNB_CMSSW_TTbar

# for ttbar in NoPU PU35 PU70 
# do
#     for bV in "BH bh" "STD std" "CE ce"
#     do echo $bV | while read -r bN bO
# 	do
# 	    oBase=${base}_${ttbar}_${bN}
# 	    echo "${oBase}: validation [nTH:24, nVU:8]"
# 	    ./mkFit/mkFit --cmssw-seeds --geom CMS-2017 --cmssw-val --ext-rec-tracks --read --file-name ${dir}/${!ttbar}/${file} --build-${bO} --num-thr 24 >& log_${oBase}_NVU8int_NTH24_cmsswval.txt
# 	    mv valtree.root valtree_${oBase}.root
# 	done
#     done
# done

make clean

for ttbar in NoPU PU35 PU70 
do
    tbase=${base}_${ttbar}
    for build in BH STD CE
    do
	root -b -q -l runValidation.C\(\"_${tbase}_${build}\",0,1\)
    done
    root -b -q -l makeValidation.C\(\"${tbase}\",1\)
done

make distclean
