#! /bin/bash

# in case this is run alone
source xeon_scripts/common_variables.sh

dir=${1:-benchmarks}

# Move benchmark plots: subroutine build benchmarks
builddir="Benchmarks"
mkdir -p ${dir}/${builddir}
mkdir -p ${dir}/${builddir}/logx

for arch in SNB KNC KNL
do
    for benchmark in TH VU
    do
	oBase=${arch}_${sample}_${benchmark}

	mv ${oBase}_"time".png ${dir}/${builddir}
	mv ${oBase}_"speedup".png ${dir}/${builddir}

	mv ${oBase}_"time_logx".png ${dir}/${builddir}/logx
	mv ${oBase}_"speedup_logx".png ${dir}/${builddir}/logx
    done
done

# Move multiple events in flight plots
meifdir="MultEvInFlight"
mkdir -p ${dir}/${meifdir}
mkdir -p ${dir}/${meifdir}/logx

for arch in SNB KNC KNL
do
    oBase=${arch}_${sample}_"MEIF"

    mv ${oBase}_"time".png ${dir}/${meifdir}
    mv ${oBase}_"speedup".png ${dir}/${meifdir}

    mv ${oBase}_"time_logx".png ${dir}/${meifdir}/logx
    mv ${oBase}_"speedup_logx".png ${dir}/${meifdir}/logx
done

# Move plots from text dump
dumpdir="PlotsFromDump"
mkdir -p ${dir}/${dumpdir}
mkdir -p ${dir}/${dumpdir}/diffs

for build in BH STD CE
do
    for var in nHits pt eta phi
    do
	mv ${sample}_${build}_${var}.png ${dir}/${dumpdir}
	mv ${sample}_${build}_"d"${var}.png ${dir}/${dumpdir}/diffs
    done
done

# Move ROOT validation
rootdir="ROOTVAL"
mkdir -p ${dir}/${rootdir}

for build in BH STD CE
do
    vBase=SNB_${sample}_${build}
    mv totals_validation_${vBase}_"ROOTVAL".txt ${dir}/${rootdir}
done

for rate in eff ineff_barrel ineff_endcap dr fr 
do
    for var in pt phi eta
    do
	mv SNB_${sample}_${rate}_${var}_*_"ROOTVAL".png ${dir}/${rootdir}
    done
done

# Move CMSSW validation
cmsswdir="CMSSWVAL"
mkdir -p ${dir}/${cmsswdir}
mkdir -p ${dir}/${cmsswdir}/diffs

for build in BH STD CE
do
    vBase=SNB_${sample}_${build}
    mv totals_validation_${vBase}_"CMSSWVAL"_cmssw.txt ${dir}/${cmsswdir}
done

for rate in eff ineff_barrel ineff_endcap dr fr 
do
    for var in pt phi eta
    do
	mv SNB_${sample}_${rate}_${var}_*_"CMSSWVAL".png ${dir}/${cmsswdir}
    done
done

for coll in bestmatch allmatch
do 
    for var in nHits invpt phi eta
    do
	mv SNB_${sample}_${coll}_d${var}_*.png ${dir}/${cmsswdir}/diffs
    done
done
