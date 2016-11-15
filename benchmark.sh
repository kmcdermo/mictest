#! /bin/bash

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make -j 12

dir=/data/nfsmic/${USER}/tmp

mkdir -p ${dir}
./mkFit/mkFit --write --file-name simtracks_10x20k.bin
mv simtracks_10x20k.bin ${dir}/

for nth in 21
do
echo "host" nth=${nth} "BH"
./mkFit/mkFit --read --file-name ${dir}/simtracks_10x20k.bin --build-bh  --num-thr ${nth} >& log_host_10x20k_BH_NVU8int_NTH${nth}.txt
echo "host" nth=${nth} "TBB"
./mkFit/mkFit --read --file-name ${dir}/simtracks_10x20k.bin --build-tbb --num-thr ${nth} --cloner-single-thread >& log_host_10x20k_TBBST_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 12
echo "host" nvu=${nvu} "BH"
./mkFit/mkFit --read --file-name ${dir}/simtracks_10x20k.bin --build-bh  --num-thr 1 >& log_host_10x20k_BH_NVU${nvu}_NTH1.txt
echo "host" nvu=${nvu} "TBB"
./mkFit/mkFit --read --file-name ${dir}/simtracks_10x20k.bin --build-tbb --num-thr 1 --cloner-single-thread >& log_host_10x20k_TBBST_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make clean
make -j 12
