#! /bin/bash

for j in 1 3 7 21
do
    echo "nTh:" ${j}
    for k in 16 32 64 128
    do
	rm log_nth${j}_nhits${k}.txt
	rm times_nth${j}_nhits${k}.txt
	
	echo "nHits/task: " ${k}
	for i in `seq 1 5`
	do
	    echo "iteration:" $i
	    ./mkFit/mkFit --find-seeds --read --file-name 20ktks_1ev_noroot.bin --build-tbb --cloner-single-thread --num-thr ${j} --hits-per-task ${k} >> log_nth${j}_nhits${k}.txt
	done
    
	grep 'Seeding:' log_nth${j}_nhits${k}.txt | cut -d ' ' -f2 > times_nth${j}_nhits${k}.txt

	root -l -b -q avgtime.C\(${j},${k}\)
    done
done