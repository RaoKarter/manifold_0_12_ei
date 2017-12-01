#!/bin/bash

OUTPUT_DIR=/home/casl/Documents/ControllerRuns

FAIL=0

echo "Running barnes"
BENCH_NAME=barnes	
./smp_llp conf_llp_10G_controller.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/old_parsec/splash2-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_Run1.txt 2>&1 &

echo "Running blackscholes"
BENCH_NAME=blackscholes
./smp_llp conf_llp_10G_controller.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_Run1.txt 2>&1 &

echo "Running streamcluster"
BENCH_NAME=streamcluster
./smp_llp conf_llp_10G_controller.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_Run1.txt 2>&1 &

for job in `jobs -p`
do
	echo $job
	wait $job || let "FAIL+=1"
done

echo $FAIL

if [ "$FAIL" == "0" ];
then
	echo "Finished barnes"
	echo "Finished blackscholes"
	echo "Finished streamcluster"
	rm /tmp/qsim_*	
else
	echo "FAIL! ($FAIL)"
	rm /tmp/qsim_*	
fi
