#! /bin/bash

make -j 12

dir=/data/nfsmic/${USER}/tmp
mkdir -p ${dir}

# Building test: Barrel
if [ ! ${dir}/simtracks_barrel_20x10k.bin ]; then
    echo "++++Generating 10k tracks/event * 20 events for ToyMC barrel-only building tests++++"
    ./mkFit/mkFit --num-thr-sim 1 --write --file-name simtracks_barrel_20x10k.bin
    mv simtracks_barrel_20x10k.bin ${dir}/
fi

# Fitting test: Barrel
if [ ! ${dir}/simtracks_barrel_1kx10k.bin ]; then
    echo "++++Generating 10k tracks/event * 1k events for ToyMC barrel-only fiting tests++++"
    ./mkFit/mkFit --num-thr-sim 1 --write --num-events 1000 --file-name simtracks_barrel_1kx10k.bin
    mv simtracks_barrel_1kx10k.bin ${dir}/
fi

# Fitting test: Endcap
if [ ! ${dir}/simtracks_endcap_1kx10k.bin ]; then
    echo "++++Generating 10k tracks/event * 1k events for ToyMC endcap-only fiting tests++++"
    ./mkFit/mkFit --num-thr-sim 1 --endcap-test --num-events 1000 --write --file-name simtracks_endcap_1kx10k.bin
    mv simtracks_endcap_1kx10k.bin ${dir}/
fi

make clean
make -j 12

# Validation tests: Barrel
if [ ! ${dir}/simtracks_barrel_10x10k_val.bin ]; then
    echo "++++Generating 10k tracks/event * 10 events for ToyMC barrel-only validation tests++++"
    sed -i 's/#WITH_ROOT := yes/WITH_ROOT := yes/g' Makefile.config
    ./mkFit/mkFit --num-thr-sim 1 --normal-val --num-events 10 --write --file-name simtracks_barrel_10x10k_val.bin
    mv simtracks_barrel_10x10k_val.bin ${dir}/
    sed -i 's/WITH_ROOT := yes/#WITH_ROOT := yes/g' Makefile.config
fi

make clean