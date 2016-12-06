#! /bin/bash

make -j 12

dir=/data/nfsmic/kmcdermo/tmp
mkdir -p ${dir}

# Building test: Barrel
if [ ! -f ${dir}/simtracks_10x20k.bin ]; then
    echo "++++Generating 20k tracks/event * 10 events for ToyMC barrel-only building tests++++"
    ./mkFit/mkFit --num-thr-sim 12 --num-events 10 --num-tracks 20000 --write --file-name simtracks_10x20k.bin
    mv simtracks_10x20k.bin ${dir}/
fi

make clean
