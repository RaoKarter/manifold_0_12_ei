#!/bin/bash

OUTPUT_DIR=/home/casl/Documents/microbenches/Data


for FREQ in 5 10 15
do
	for NCPUOPS in 1 10
	do
		for CNUM in {14..14..1}
		do
			if [ ${CNUM} -eq 14 ]; then
	#---------------------------------------------------------------------------
	# CORE 0 BANK 0, 1, 3, 15
	#---------------------------------------------------------------------------
				# L2BANK2_3_4_6 OPTMEM1_2_3_7
				#for L2BANK in 2 3 4 6
				for L2BANK in 2 13
				do
					FAIL=0
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID 15 OPTMEM7 FREQ " ${FREQ} 
					echo "CORE_NUMBER " ${CNUM} " L2BANKID " ${L2BANK} "OPTMEM1 FREQ " ${FREQ}
					BENCH_NAME=c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem1
					./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID 15 OPTMEM7 FREQ " ${FREQ} 
					echo "CORE_NUMBER " ${CNUM} " L2BANKID " ${L2BANK} "OPTMEM2 FREQ " ${FREQ}
					BENCH_NAME=c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem2
					./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID 15 OPTMEM7 FREQ " ${FREQ} 
					echo "CORE_NUMBER " ${CNUM} " L2BANKID " ${L2BANK} "OPTMEM3 FREQ " ${FREQ}
					BENCH_NAME=c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem3
					./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID 15 OPTMEM7 FREQ " ${FREQ} 
					echo "CORE_NUMBER " ${CNUM} " L2BANKID " ${L2BANK} "OPTMEM7 FREQ " ${FREQ}
					BENCH_NAME=c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem7
					./smp_llp conf_llp_10G_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

					for job in `jobs -p`
					do
						echo $job
						wait $job || let "FAIL+=1"
					done

					echo $FAIL

					if [ "$FAIL" == "0" ];
					then
						echo "Finished c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem1_freq${FREQ}"
						echo "Finished c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem2_freq${FREQ}"
						echo "Finished c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem3_freq${FREQ}"
						echo "Finished c14_b2_c13_b${L2BANK}_cpuop${NCPUOPS}_optmem7_freq${FREQ}"
						rm /tmp/qsim_*	
					else
						echo "FAIL! ($FAIL)"
					fi
				done
			fi

			if [ ${CNUM} -eq 0 ]; then
	#---------------------------------------------------------------------------- 
	# CORE 2 BANK 0, 2, 3, 13
	#---------------------------------------------------------------------------
				# L2BANK0_3_5_6 OPTMEM1_2_3_7
				for L2BANK in 0 3 5 6
				do
					FAIL=0
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM1 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem1
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM2 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem2
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM3 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem3
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM7 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem7
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

					for job in `jobs -p`
					do
						echo $job
						wait $job || let "FAIL+=1"
					done

					echo $FAIL

					if [ "$FAIL" == "0" ];
					then
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem1_freq${FREQ}"
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem2_freq${FREQ}"
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem3_freq${FREQ}"
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem7_freq${FREQ}"
						rm /tmp/qsim_*	
					else
						echo "FAIL! ($FAIL)"
					fi
				done
			fi
			if [ ${CNUM} -eq 1 ]; then 
	#----------------------------------------------------------------------------
	# CORE 3 BANK 0, 1, 3, 5, 6
	#---------------------------------------------------------------------------
				# L2BANK3_4_6_8_9 OPTMEM1_2_3_7
				for L2BANK in 3 4 6 8 9
				do
					FAIL=0
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM1 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem1
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM2 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem2
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM3 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem3
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &
					echo "Starting NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM7 FREQ ${FREQ}"
					BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem7
					./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1 &

					for job in `jobs -p`
					do
						echo $job
						wait $job || let "FAIL+=1"
					done

					echo $FAIL

					if [ "$FAIL" == "0" ];
					then
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem1_freq${FREQ}"
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem2_freq${FREQ}"
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem3_freq${FREQ}"
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem7_freq${FREQ}"
						rm /tmp/qsim_*	
					else
						echo "FAIL! ($FAIL)"
					fi
				done
			fi
		done
	done

done
