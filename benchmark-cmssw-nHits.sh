#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h
patch < cmssw-config.patch

make -j 12

dir=nfsmic/cerati

for nth in 24
do
    echo "SNB CMSSW" nth=${nth} "BH (Barrel)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr ${nth} >& log_SNB_CMSSW_Barrel_BH_NVU8int_NTH${nth}.txt
    echo "SNB CMSSW" nth=${nth} "CE (Barrel)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-ce  --num-thr ${nth} --cloner-single-thread >& log_SNB_CMSSW_Barrel_CE_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1
do
    sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
    make clean
    make -j 12

    echo "SNB CMSSW" nvu=${nvu} "BH (Barrel)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr 1 >& log_SNB_CMSSW_Barrel_BH_NVU${nvu}_NTH1.txt
    echo "SNB CMSSW" nvu=${nvu} "CE (Barrel)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-ce  --num-thr 1 --cloner-single-thread >& log_SNB_CMSSW_Barrel_CE_NVU${nvu}_NTH1.txt

    sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

make clean
########
make -j 12

for nth in 240
do
    echo "KNC CMSSW" nth=${nth} "BH (Barrel)"
    ssh mic0 ./mkFit-mic --read --file-name /${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr ${nth} >& log_KNC_CMSSW_Barrel_BH_NVU16int_NTH${nth}.txt
    echo "KNC CMSSW" nth=${nth} "CE (Barrel)"
    ssh mic0 ./mkFit-mic --read --file-name /${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-ce  --seeds-per-task 32 --num-thr ${nth} --cloner-single-thread >& log_KNC_CMSSW_Barrel_CE_NVU16int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1
do
    sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
    make clean
    make -j 12
   
    echo "KNC CMSSW" nvu=${nvu} "BH (Barrel)"
    ssh mic0 ./mkFit-mic --read --file-name /${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr 1 >& log_KNC_CMSSW_Barrel_BH_NVU${nvu}_NTH1.txt
    echo "KNC CMSSW" nvu=${nvu} "CE (Barrel)"
    ssh mic0 ./mkFit-mic --read --file-name /${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-ce  --seeds-per-task 32 --num-thr 1 --cloner-single-thread >& log_KNC_CMSSW_Barrel_CE_NVU${nvu}_NTH1.txt

    sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

patch -R < cmssw-config.patch
sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make distclean

for test in BH CE; do
    echo "Making nHits plots for CMSSW Barrel:" ${test}
    python makePlotsFromDump.py SNB CMSSW Barrel ${test} NVU1_NTH1
    python makePlotsFromDump.py SNB CMSSW Barrel ${test} NVU8int_NTH24
    python makePlotsFromDump.py KNC CMSSW Barrel ${test} NVU1_NTH1
    python makePlotsFromDump.py KNC CMSSW Barrel ${test} NVU16int_NTH240
    root -b -q -l makePlotsFromDump.C\(\"${test}\",1\)
done
