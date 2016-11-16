#! /bin/bash

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h
patch < cmssw-endcap-config.patch

make -j 12

dir=/data/nfsmic/cerati/

for nth in 1 2 4 6 8 12 16 20 24
do
    echo "snb cmssw" nth=${nth} "BH (endcap)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-bh   --num-thr ${nth} >& log_snb_endcap_100xTTbarPU35_BH_NVU8int_NTH${nth}.txt
    echo "snb cmssw" nth=${nth} "COMB (endcap)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-comb --num-thr ${nth} --cloner-single-thread >& log_snb_endcap_100xTTbarPU35_COMB_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8
do
    sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
    make clean
    make -j 12

    echo "snb cmssw" nvu=${nvu} "BH (endcap)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-bh   --num-thr 12 >& log_snb_endcap_100xTTbarPU35_BH_NVU${nvu}_NTH12.txt
    echo "snb cmssw" nvu=${nvu} "TBB (endcap)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-comb --num-thr 12 --cloner-single-thread >& log_snb_endcap_100xTTbarPU35_COMB_NVU${nvu}_NTH12.txt

    sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

patch -R < cmssw-endcap-config.patch
sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make clean
