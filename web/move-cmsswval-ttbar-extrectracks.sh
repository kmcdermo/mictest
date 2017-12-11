#!/bin/bash

dir=${1:-plots}
outdir=${dir}/cmssw-val-hit-cmsdenom-0.8-OR-trkparam
base=SNB_CMSSW_TTbar

echo "Moving plots and text files locally to ${outdir}"
for ttbar in NoPU PU35 PU70 
do
    fulldir=${outdir}/${ttbar}
    mkdir -p ${fulldir}
    mkdir -p ${fulldir}/diffs

    for rate in eff ineff_barrel ineff_endcap fr dr
    do
	for var in pt eta phi
	do
	    mv ${base}_${ttbar}_${rate}_${var}_*.png ${fulldir}
	done
    done

    for coll in bestmatch allmatch
    do 
	for var in nHits invpt phi eta
	do
	    mv ${base}_${ttbar}_${coll}_d${var}_*.png ${fulldir}/diffs
	done
    done

    for build in BH STD CE
    do
	vbase=validation_${base}_${ttbar}_${build}
	mv ${vbase}/totals_${vbase}_cmssw.txt ${fulldir}
    done
done

host=kmcdermo@lxplus.cern.ch
whost=${host}":~/www"
echo "Moving plots and text files remotely to ${whost}"
scp -r ${dir} ${whost}

echo "Executing remotely ./makereadable.sh ${outdir}"
ssh ${host} bash -c "'
cd www
./makereadable.sh ${outdir}
exit
'"

echo "Removing local files"
for ttbar in NoPU PU35 PU70
do
    for build in BH STD CE
    do
	testbase=${base}_${ttbar}_${build}
	rm -rf validation_${testbase}
	rm -rf log_${testbase}_NVU8int_NTH24_cmsswval.txt 
    done
done

rm -rf ${dir}
