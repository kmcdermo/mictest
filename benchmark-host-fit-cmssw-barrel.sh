#! /bin/bash

dir=/data/nfsmic/cerati/

patch < cmssw-config.patch

make -j 8

for nth in 1 3 7 21
do
echo "host cmssw" nth=${nth} "FIT"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --fit-sort-only --num-thr ${nth} >& log_host_100xTTbarPU35_FIT_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 8
echo "host" nvu=${nvu} "FIT"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --fit-sort-only --num-thr 1 >& log_host_100xTTbarPU35_FIT_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

patch -R < cmssw-config.patch

make clean
make -j 8
