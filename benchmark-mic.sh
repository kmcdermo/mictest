#! /bin/bash

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make -j 8

dir=/data/nfsmic/${USER}/tmp
micdir=/nfsmic/${USER}/tmp

mkdir -p ${dir}
./mkFit/mkFit --write --file-name simtracks_10x20k.bin
mv simtracks_10x20k.bin ${dir}/

for nth in 1 3 5 15 30 45 60 75 90 105 120 150 180 210 240
do
echo "mic" nth=${nth} "BH"
ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_10x20k.bin --build-bh  --num-thr ${nth} >& log_mic_10x20k_BH_NVU16int_NTH${nth}.txt
echo "mic" nth=${nth} "TBB"
ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_10x20k.bin --build-tbb --seeds-per-task 32 --num-thr ${nth} --cloner-single-thread >& log_mic_10x20k_TBBST_NVU16int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8 16
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 8
echo "mic" nvu=${nvu} "BH"
ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_10x20k.bin --build-bh  --num-thr 1 >& log_mic_10x20k_BH_NVU${nvu}_NTH1.txt
echo "mic" nvu=${nvu} "TBB"
ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_10x20k.bin --build-tbb --seeds-per-task 32 --num-thr 1 --cloner-single-thread >& log_mic_10x20k_TBBST_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make clean
make -j 8
