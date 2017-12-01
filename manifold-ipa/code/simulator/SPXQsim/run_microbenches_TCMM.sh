#!/bin/bash

OUTPUT_DIR=/home/casl/Documents/microbenchOutput

: << END
for FREQ in 5 10 15
do

#---------------- L2 BANK 15 ------------------------#
		FAIL=0
		echo "Starting NCPUOPS 1 OPTMEM 1 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop1_optmem1
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 1 OPTMEM 2 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop1_optmem2
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 1 OPTMEM 3 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop1_optmem3
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 1 OPTMEM 7 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop1_optmem7
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 1 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop10_optmem1
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 2 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop10_optmem2
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 3 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop10_optmem3
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 7 C0_B15_C15_B15 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b15_cpuop10_optmem7
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		for job in `jobs -p`
		do
			echo $job
			wait $job || let "FAIL+=1"
		done

		echo $FAIL

		if [ "$FAIL" == "0" ];
		then
			echo "Finished c0_b15_c15_b15_cpuop1_optmem1_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop1_optmem2_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop1_optmem3_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop1_optmem7_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop10_optmem1_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop10_optmem2_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop10_optmem3_freq${FREQ}"
			echo "Finished c0_b15_c15_b15_cpuop10_optmem7_freq${FREQ}"
			rm /tmp/qsim_*	
		else
			echo "FAIL! ($FAIL)"
		fi
done
END
#: << END
for FREQ in 5 10 15
do
#---------------- L2 BANK 9 -------------------------#
		FAIL=0
		echo "Starting NCPUOPS 1 OPTMEM 1 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop1_optmem1
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 1 OPTMEM 2 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop1_optmem2
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 1 OPTMEM 3 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop1_optmem3
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 1 OPTMEM 7 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop1_optmem7
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 1 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop10_optmem1
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 2 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop10_optmem2
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 3 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop10_optmem3
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		echo "Starting NCPUOPS 10 OPTMEM 3 C0_B15_C15_B9 FREQ " ${FREQ} 
		BENCH_NAME=c0_b15_c15_b9_cpuop10_optmem7
		./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

		for job in `jobs -p`
		do
			echo $job
			wait $job || let "FAIL+=1"
		done

		echo $FAIL

		if [ "$FAIL" == "0" ];
		then
			echo "Finished c0_b15_c15_b9_cpuop1_optmem1_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop1_optmem2_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop1_optmem3_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop1_optmem7_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop10_optmem1_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop10_optmem2_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop10_optmem3_freq${FREQ}"
			echo "Finished c0_b15_c15_b9_cpuop10_optmem7_freq${FREQ}"
			rm /tmp/qsim_*	
		else
			echo "FAIL! ($FAIL)"
		fi
done
#END
#cd /home/casl/Documents/microbenchOutput
#./power_temperature_extraction_TCMM.sh

