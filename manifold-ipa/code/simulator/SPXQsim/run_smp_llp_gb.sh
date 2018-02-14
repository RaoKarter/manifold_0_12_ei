#!/bin/bash

QSIM_TESTER_DIR=/home/casl/Documents/qsim-0.1.5.1/examples
QSIM_MAIN_DIR=/home/casl/Documents/qsim-0.1.5.1
OUTPUT_DIR=/home/casl/Documents/graphbig_qsim

: << END
FAIL=0
echo "Running bfs on qsim tester"
BENCHNAME=bfs
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running bc on qsim tester"
BENCHNAME=bc
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running connectedcomponent on qsim tester"
BENCHNAME=connectedcomponent
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running dc on qsim tester"
BENCHNAME=dc
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running dfs on qsim tester"
BENCHNAME=dfs
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running graphcoloring on qsim tester"
BENCHNAME=graphcoloring
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running graphupdate on qsim tester"
BENCHNAME=graphupdate
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

echo "Running kcore on qsim tester"
BENCHNAME=kcore
${QSIM_TESTER_DIR}/tester_new 16 /dev/null ${QSIM_MAIN_DIR}/state.16 ${BENCHNAME}.tar > ${BENCHNAME}_qsimtester.txt 2>&1 &

for job in `jobs -p`
do
	echo $job
	wait $job || let "FAIL+=1"
done

echo $FAIL

if [ "$FAIL" == "0" ];
then
	echo "Finished bfs"
	echo "Finished bc"
	echo "Finished cc"
	echo "Finished dc"
	echo "Finished dfs"
	echo "Finished gc"
	echo "Finished gu"
	echo "Finished kc"
	rm /tmp/qsim_*	
else
	echo "FAIL! ($FAIL)"
	rm /tmp/qsim_*	
fi

END
FAIL=0

echo "Running connectedcomponent 5G"
BENCH_NAME=connectedcomponent	
./smp_llp conf_llp_5G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_5G.txt 2>&1 &

echo "Running connectedcomponent 10G"
BENCH_NAME=connectedcomponent
./smp_llp conf_llp_10G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_10G.txt 2>&1 &

echo "Running connectedcomponent 15G"
BENCH_NAME=connectedcomponent
./smp_llp conf_llp_15G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_15G.txt 2>&1 &

echo "Running connectedcomponent 20G"
BENCH_NAME=connectedcomponent
./smp_llp conf_llp_20G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss /home/casl/Documents/graphbig_qsim/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_20G.txt 2>&1 &

for job in `jobs -p`
do
	echo $job
	wait $job || let "FAIL+=1"
done

echo $FAIL

if [ "$FAIL" == "0" ];
then
	echo "Finished cc 5G"
	echo "Finished cc 10G"
	echo "Finished cc 15G"
	echo "Finished cc 20G"
	rm /tmp/qsim_*	
else
	echo "FAIL! ($FAIL)"
	rm /tmp/qsim_*	
fi
#END
