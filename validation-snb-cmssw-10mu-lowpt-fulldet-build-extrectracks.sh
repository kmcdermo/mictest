#! /bin/bash

make -j 12 WITH_ROOT=yes

dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/10muLowPt
file=memoryFile.fv3.recT.072617.bin

base=SNB_CMSSW_10mu_lowpt

for bV in "BH bh" "STD std" "CE ce"
do echo $bV | while read -r bN bO
    do
	oBase=${base}_${bN}
	    echo "${oBase}: validation [nTH:24, nVU:8]"
	    ./mkFit/mkFit --cmssw-seeds --clean-seeds --geom CMS-2017 --cmssw-val --ext-rec-tracks --read --file-name ${dir}/${file} --build-${bO} --num-thr 24 >& log_${oBase}_NVU8int_NTH24_cmsswval.txt
	    mv valtree.root valtree_${oBase}.root
    done
done

make clean

for build in BH STD CE
do
    root -b -q -l runValidation.C\(\"_${base}_${build}\",0,1\)
done

root -b -q -l makeValidation.C\(\"${base}\",1\)

make distclean
