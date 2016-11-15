#! /bin/bash

dir=/nfsmic/cerati/

patch < cmssw-config.patch

make -j 12

for nth in 210
do
echo "mic cmssw" nth=${nth} "BH"
ssh mic0 ./mkFit-mic --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr ${nth} >& log_mic_100xTTbarPU35_BH_NVU16int_NTH${nth}.txt
echo "mic cmssw" nth=${nth} "TBB"
ssh mic0 ./mkFit-mic --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-tbb --seeds-per-task 32 --num-thr ${nth} --cloner-single-thread >& log_mic_100xTTbarPU35_TBBST_NVU16int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 12
echo "mic cmssw" nvu=${nvu} "BH"
ssh mic0 ./mkFit-mic --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr 1 >& log_mic_100xTTbarPU35_BH_NVU${nvu}_NTH1.txt
echo "mic cmssw" nvu=${nvu} "TBB"
ssh mic0 ./mkFit-mic --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-tbb --seeds-per-task 32 --num-thr 1 --cloner-single-thread >& log_mic_100xTTbarPU35_TBBST_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

patch -R < cmssw-config.patch

make clean
make -j 12
