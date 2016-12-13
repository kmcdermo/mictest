#! /bin/bash

[ -z "$ROOTSYS" ] && source ~matevz/root/bin/thisroot.sh

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h
patch < cmssw-endcap-config.patch

make -j 12

dir=nfsmic/cerati

for nth in 24
do
    echo "SNB CMSSW" nth=${nth} "BH (Endcap)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-bh  --num-thr ${nth} >& log_SNB_CMSSW_Endcap_BH_NVU8int_NTH${nth}.txt
    echo "SNB CMSSW" nth=${nth} "CE (Endcap)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-ce  --num-thr ${nth} --cloner-single-thread >& log_SNB_CMSSW_Endcap_CE_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1
do
    sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
    make clean
    make -j 12

    echo "SNB CMSSW" nvu=${nvu} "BH (Endcap)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-bh  --num-thr 1 >& log_SNB_CMSSW_Endcap_BH_NVU${nvu}_NTH1.txt
    echo "SNB CMSSW" nvu=${nvu} "CE (Endcap)"
    ./mkFit/mkFit --read --file-name /data/${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-ce  --num-thr 1 --cloner-single-thread >& log_SNB_CMSSW_Endcap_CE_NVU${nvu}_NTH1.txt

    sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

patch -R < cmssw-endcap-config.patch
sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make distclean

for test in BH CE; do
    echo "Making nHits plots for CMSSW Endcap:" ${test}
    python makePlotsFromDump.py SNB CMSSW Endcap ${test} NVU1_NTH1
    python makePlotsFromDump.py SNB CMSSW Endcap ${test} NVU8int_NTH24
    root -b -q -l makePlotsFromDump.C\(\"${test}\",1,1\)
done
