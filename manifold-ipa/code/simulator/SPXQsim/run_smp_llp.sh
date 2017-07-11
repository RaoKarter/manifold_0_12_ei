#!/bin/bash

OUTPUT_DIR=/media/karthik/MontFort_part2/MainUbuntuKarthikFiles/TemperatureWork/Data/NoController


for FREQ in {5..30..5}
do
	BENCH_NAME=blackscholes
	echo $FREQ
	echo "Benchmark: " ${BENCH_NAME}
	echo 
	echo "Output directory: " ${OUTPUT_DIR}
    ./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 16core_ooo.config.ss old_parsec/parsec-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1

	BENCH_NAME=raytrace

	echo $FREQ
	echo "Benchmark: " ${BENCH_NAME}
	echo 
	echo "Output directory: " ${OUTPUT_DIR}
    ./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 16core_ooo.config.ss old_parsec/splash2-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1

	BENCH_NAME=barnes

	echo $FREQ
	echo "Benchmark: " ${BENCH_NAME}
	echo 
	echo "Output directory: " ${OUTPUT_DIR}
    ./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 16core_ooo.config.ss old_parsec/splash2-tar/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1

done
