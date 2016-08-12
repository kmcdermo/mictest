#! /bin/bash


dir=/data/nfsmic/cerati/

patch < cmssw-endcap-config.patch

make -j 24

for nth in 1 3
do
echo "host cmssw" nth=${nth} "BH (endcap)"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-bh  --num-thr ${nth} >& log_host_endcap_100xTTbarPU35_BH_NVU8int_NTH${nth}.txt
echo "host cmssw" nth=${nth} "STD (endcap)"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-std --num-thr ${nth} >& log_host_endcap_100xTTbarPU35_ST_NVU8int_NTH${nth}.txt
echo "host cmssw" nth=${nth} "CE (endcap)"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-ce  --num-thr ${nth} >& log_host_endcap_100xTTbarPU35_CE_NVU8int_NTH${nth}.txt
echo "host cmssw" nth=${nth} "CEST (endcap)"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-ce  --num-thr ${nth} --cloner-single-thread >& log_host_endcap_100xTTbarPU35_CEST_NVU8int_NTH${nth}.txt
echo "host cmssw" nth=${nth} "TBB (endcap)"
./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_endcapEta1p7.bin --endcap-test --cms-geom --cmssw-seeds --build-tbb --num-thr ${nth} --cloner-single-thread >& log_host_endcap_100xTTbarPU35_TBBST_NVU8int_NTH${nth}.txt
done

patch -R < cmssw-endcap-config.patch

make clean
make -j 24
