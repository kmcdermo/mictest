#! /bin/bash

outdir=${1:-plots}
mkdir -p ${outdir}

make -j 12 WITH_ROOT=yes

dir=/data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep
file=memoryFile.fv3.recT.072617.bin

ECN2=${dir}/10muEta-24to-17Pt1to10/${file}
ECN1=${dir}/10muEta-175to-055Pt1to10/${file}
BRL=${dir}/10muEtaLT06Pt1to10/${file}
ECP1=${dir}/10muEta055to175Pt1to10/${file}
ECP2=${dir}/10muEta17to24Pt1to10/${file}

for section in ECN2 ECN1 BRL ECP1 ECP2 
do
    echo ${section}
    echo "  Building"
    ./mkFit/mkFit --ext-rec-tracks --geom CMS-2017 --read --file-name ${!section} --build-ce --cmssw-seeds --num-thr 8 >& ${outdir}/dump_${section}.txt
    echo "  Hadding then removing extra files" 
    hadd ${outdir}/dump${section}.root dump_*.root >& ${outdir}/hadd_${section}.txt
    rm dump_*.root
done

rm ${outdir}/*.txt

#make distclean
