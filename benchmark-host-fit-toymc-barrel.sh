#! /bin/bash

make -j 8

dir=/data/nfsmic/${USER}/tmp

mkdir -p ${dir}
./mkFit/mkFit --write --file-name simtracks_matchcmssw.bin
mv simtracks_matchcmssw.bin ${dir}/

for nth in 1 2 3 6 8 12 16 20 24
do
echo "host" nth=${nth} "FIT"
./mkFit/mkFit --read --file-name ${dir}/simtracks_matchcmssw.bin --fit-fake-only --num-thr ${nth} >& log_host_matchcmssw_FIT_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 8
echo "host" nvu=${nvu} "FIT"
./mkFit/mkFit --read --file-name ${dir}/simtracks_matchcmssw.bin --fit-fake-only --num-thr 1 >& log_host_matchcmssw_FIT_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

make clean
make -j 8
