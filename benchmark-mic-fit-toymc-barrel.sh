#! /bin/bash

make -j 8

dir=/data/nfsmic/${USER}/tmp
micdir=/nfsmic/${USER}/tmp

mkdir -p ${dir}
./mkFit/mkFit --write --file-name simtracks_matchcmssw.bin
mv simtracks_matchcmssw.bin ${dir}/

for nth in 1 2 3 5 10 15 20 30 45 60 75 90 105 120 150 180 210 240
do
echo "mic" nth=${nth} "FIT"
ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_matchcmssw.bin --fit-fake-only --num-thr ${nth} >& log_mic_matchcmssw_FIT_NVU16int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8 16
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 8
echo "mic" nvu=${nvu} "FIT"
ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_matchcmssw.bin --fit-fake-only --num-thr 1 >& log_mic_matchcmssw_FIT_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

make clean
make -j 8
