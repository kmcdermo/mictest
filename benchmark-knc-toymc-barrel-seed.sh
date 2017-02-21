#! /bin/bash

make -j 12

micdir=/nfsmic/kmcdermo/toymc/seedtests/

for nth in 1 2 4 8 15 30 60 90 120 150 180 210 240
do 
    echo "KNC ToyMC" nth=${nth} "Seed (Barrel): 1000 Tk/Ev * 100 Ev"
    ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_1kx100.bin --num-tracks 1000  --build-bh --find-seeds --num-thr ${nth} >& log.txt
    echo "KNC ToyMC" nth=${nth} "Seed (Barrel): 5000 Tk/Ev * 20 Ev"
    ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_5kx20.bin  --num-tracks 5000  --build-bh --find-seeds --num-thr ${nth} >& log.txt
    echo "KNC ToyMC" nth=${nth} "Seed (Barrel): 10000 Tk/Ev * 10 Ev"
    ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_10kx10.bin --num-tracks 10000 --build-bh --find-seeds --num-thr ${nth} >& log.txt
done

# sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
# for nvu in 1 2 4 8 16
# do
#     sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
#     make clean
#     make -j 12
    
#     echo "SNB ToyMC" nvu=${nvu} "Seed (Barrel): 1000 Tk/Ev * 100 Ev"
#     ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_1kx100.bin --num-tracks 1000  --build-bh --find-seeds --num-thr 1 >& log.txt
#     echo "SNB ToyMC" nvu=${nvu} "Seed (Barrel): 5000 Tk/Ev * 20 Ev"
#     ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_5kx20.bin  --num-tracks 5000  --build-bh --find-seeds --num-thr 1 >& log.txt
#     echo "SNB ToyMC" nvu=${nvu} "Seed (Barrel): 10000 Tk/Ev * 10 Ev"
#     ssh mic0 ./mkFit-mic --read --file-name ${micdir}/simtracks_10kx10.bin --num-tracks 10000 --build-bh --find-seeds --num-thr 1 >& log.txt

#     sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
# done
# sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

make clean

