#!/bin/bash

OUTPUT_DIR=/home/casl/Documents/ProfilingRuns/DataForSC

for FREQ in 5 10 15 20 25 30
do
	FAIL=0
	echo "Running kcore"
	BENCH_NAME=kcore	
	./smp_llp conf_llp_1core_${FREQ}G_DDR1.cfg.bst.ss outorder.config.ss qsim_lib ~/Downloads/qsim-0.1.5/state.1 1core_ooo.config.ss /home/casl/Documents/gb/qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_1core_${FREQ}G_DDR1.txt 2>&1 &

	echo "Running blackscholes"
	BENCH_NAME=blackscholes
	./smp_llp conf_llp_1core_${FREQ}G_DDR1.cfg.bst.ss outorder.config.ss qsim_lib ~/Downloads/qsim-0.1.5/state.1 1core_ooo.config.ss /home/casl/Documents/old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_1core_${FREQ}G_DDR1.txt 2>&1 &

	echo "Running streamcluster"
	BENCH_NAME=streamcluster
	./smp_llp conf_llp_1core_${FREQ}G_DDR1.cfg.bst.ss outorder.config.ss qsim_lib ~/Downloads/qsim-0.1.5/state.1 1core_ooo.config.ss /home/casl/Documents/old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_1core_${FREQ}G_DDR1.txt 2>&1 &

	echo "Running pagerank"
	BENCH_NAME=pagerank
	./smp_llp conf_llp_1core_${FREQ}G_DDR1.cfg.bst.ss outorder.config.ss qsim_lib ~/Downloads/qsim-0.1.5/state.1 1core_ooo.config.ss /home/casl/Documents/gb/qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_1core_${FREQ}G_DDR1.txt 2>&1 &

	echo "Running tc"
	BENCH_NAME=tc
	./smp_llp conf_llp_1core_${FREQ}G_DDR1.cfg.bst.ss outorder.config.ss qsim_lib ~/Downloads/qsim-0.1.5/state.1 1core_ooo.config.ss /home/casl/Documents/gb/qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_1core_${FREQ}G_DDR1.txt 2>&1 &

	echo "Running barnes"
	BENCH_NAME=barnes	
	./smp_llp conf_llp_1core_${FREQ}G_DDR1.cfg.bst.ss outorder.config.ss qsim_lib ~/Downloads/qsim-0.1.5/state.1 1core_ooo.config.ss /home/casl/Documents/old_parsec/splash2-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_1core_${FREQ}G_DDR1.txt 2>&1 &

	for job in `jobs -p`
	do
		echo $job
		wait $job || let "FAIL+=1"
	done

	echo $FAIL

	if [ "$FAIL" == "0" ];
	then	
		echo "Finished kcore ${FREQ} DDR1"
		echo "Finished pagerank ${FREQ} DDR1"
		echo "Finished tc ${FREQ} DDR1"
		echo "Finished barnes ${FREQ} DDR1"
		echo "Finished blackscholes ${FREQ} DDR1"
		echo "Finished streamcluster ${FREQ} DDR1"
		rm /tmp/qsim_*	
	else
		echo "FAIL! ($FAIL)"
		rm /tmp/qsim_*	
	fi
done
