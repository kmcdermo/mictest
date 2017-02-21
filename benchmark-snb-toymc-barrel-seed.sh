#! /bin/bash

make clean
make -j 12

dir=/data/nfsmic/kmcdermo/toymc/seedtests/

for nth in 1 2 4 6 8 12 16 20 24
do 
    echo "SNB ToyMC" nth=${nth} "Seed (Barrel): 1000 Tk/Ev * 100 Ev"
    ./mkFit/mkFit --read --file-name ${dir}/simtracks_1kx100.bin --num-tracks 1000  --build-bh --find-seeds --num-thr ${nth} >& log.txt
    echo "SNB ToyMC" nth=${nth} "Seed (Barrel): 5000 Tk/Ev * 20 Ev"
    ./mkFit/mkFit --read --file-name ${dir}/simtracks_5kx20.bin  --num-tracks 5000  --build-bh --find-seeds --num-thr ${nth} >& log.txt
    echo "SNB ToyMC" nth=${nth} "Seed (Barrel): 10000 Tk/Ev * 10 Ev"
    ./mkFit/mkFit --read --file-name ${dir}/simtracks_10kx10.bin --num-tracks 10000 --build-bh --find-seeds --num-thr ${nth} >& log.txt
done

# sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
# for nvu in 1 2 4 8
# do
#     sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
#     make clean
#     make -j 12
    
#     echo "SNB ToyMC" nvu=${nvu} "Seed (Barrel): 1000 Tk/Ev * 100 Ev"
#     ./mkFit/mkFit --read --file-name ${dir}/simtracks_1kx100.bin --num-tracks 1000  --build-bh --find-seeds --num-thr 1 >& log.txt
#     echo "SNB ToyMC" nvu=${nvu} "Seed (Barrel): 5000 Tk/Ev * 20 Ev"
#     ./mkFit/mkFit --read --file-name ${dir}/simtracks_5kx20.bin  --num-tracks 5000  --build-bh --find-seeds --num-thr 1 >& log.txt
#     echo "SNB ToyMC" nvu=${nvu} "Seed (Barrel): 10000 Tk/Ev * 10 Ev"
#     ./mkFit/mkFit --read --file-name ${dir}/simtracks_10kx10.bin --num-tracks 10000 --build-bh --find-seeds --num-thr 1 >& log.txt

#     sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
# done
# sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

make clean

