#!/bin/bash

OUTPUT_DIR=/home/casl/Documents/ProfilingRuns

for FREQ in 5 15
do
	FAIL=0

	echo "Running kcore"
	BENCH_NAME=kcore	
	./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_ProfRun1_${FREQ}.txt 2>&1 &

	echo "Running pagerank"
	BENCH_NAME=pagerank
	./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_ProfRun1_${FREQ}.txt 2>&1 &

	echo "Running tc"
	BENCH_NAME=tc
	./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_ProfRun1_${FREQ}.txt 2>&1 &

	echo "Running barnes"
	BENCH_NAME=barnes	
	./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/old_parsec/splash2-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_ProfRun1_${FREQ}.txt 2>&1 &

	echo "Running blackscholes"
	BENCH_NAME=blackscholes
	./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_ProfRun1_${FREQ}.txt 2>&1 &

	echo "Running streamcluster"
	BENCH_NAME=streamcluster
	./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_ProfRun1_${FREQ}.txt 2>&1 &

	for job in `jobs -p`
	do
		echo $job
		wait $job || let "FAIL+=1"
	done

	echo $FAIL

	if [ "$FAIL" == "0" ];
	then	
		echo "Finished kcore ${FREQ}"
		echo "Finished pagerank ${FREQ}"
		echo "Finished tc ${FREQ}"
		echo "Finished barnes ${FREQ}"
		echo "Finished blackscholes ${FREQ}"
		echo "Finished streamcluster ${FREQ}"
		rm /tmp/qsim_*	
	else
		echo "FAIL! ($FAIL)"
		rm /tmp/qsim_*	
	fi
done
