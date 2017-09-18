#!/bin/bash

OUTPUT_DIR=/media/karthik/MontFort_part2/MainUbuntuKarthikFiles/TemperatureWork/Data/microbenchOutput/CharacterizationData


for FREQ in {5..15..1}
do
	for NCPUOPS in 1 10
	do
		for CNUM in 0 1 14
		do
			if [ ${CNUM} -eq 14 ]; then
	#---------------------------------------------------------------------------
	# CORE 0 BANK 0, 1, 3, 15
	#---------------------------------------------------------------------------
				# L2BANK2_3_4_6 OPTMEM1_2_3_7
				for L2BANK in 2 3 4 6
				do
					for NUMOPT in 1 2 3 7
					do
						echo "NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM${NUMOPT} FREQ ${FREQ}"
						BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem${NUMOPT}
						./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem${NUMOPT}_freq${FREQ}"
						rm /tmp/qsim_*				
					done
				done
			fi

			if [ ${CNUM} -eq 0 ]; then
	#---------------------------------------------------------------------------- 
	# CORE 2 BANK 0, 2, 3, 13
	#---------------------------------------------------------------------------
				# L2BANK0_3_5_6 OPTMEM1_2_3_7
				for L2BANK in 0 3 5 6
				do
					for NUMOPT in 1 2 3 7
					do
						echo "NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM${NUMOPT} FREQ ${FREQ}"
						BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem${NUMOPT}
						./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem${NUMOPT}_freq${FREQ}"
						rm /tmp/qsim_*
					done
				done
			fi
			if [ ${CNUM} -eq 1 ]; then 
	#----------------------------------------------------------------------------
	# CORE 3 BANK 0, 1, 3, 5, 6
	#---------------------------------------------------------------------------
				# L2BANK3_4_6_8_9 OPTMEM1_2_3_7
				for L2BANK in 3 4 6 8 9
				do
					for NUMOPT in 1 2 3 7
					do
						echo "NCPUOPS " ${NCPUOPS} " CORE_NUMBER " ${CNUM} " L2BANKID ${L2BANK} OPTMEM${NUMOPT} FREQ ${FREQ}"
						BENCH_NAME=core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem${NUMOPT}
						./smp_llp conf_llp_${FREQ}G.cfg.bst.ss outorder.config.ss qsim_lib qsim-0.1.5.1/state.16 Full3D_16core_ooo.config.ss microbenches/tarballs/core${CNUM}/${BENCH_NAME}.tar > ${OUTPUT_DIR}/${BENCH_NAME}_${FREQ}.txt 2>&1
						echo "Finished core${CNUM}_bank${L2BANK}_cpuop${NCPUOPS}_optmem${NUMOPT}_freq${FREQ}"
						rm /tmp/qsim_*
					done
				done
			fi
		done
	done

done
