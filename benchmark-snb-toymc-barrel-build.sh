#! /bin/bash

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make -j 12

dir=/data/nfsmic/kmcdermo/tmp

for nth in 1 3 7 21
do 
    echo "snb toymc" nth=${nth} "STD (barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/simtracks_barrel_10x20k.bin --build-std --num-thr ${nth} >& log_snb_10x20k_STD_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8
do
    sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
    make clean
    make -j 12
    
    echo "snb toymc" nvu=${nvu} "STD (barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/simtracks_barrel_10x20k.bin --build-std --num-thr 1 >& log_snb_10x20k_STD_NVU${nvu}_NTH1.txt

    sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make clean

