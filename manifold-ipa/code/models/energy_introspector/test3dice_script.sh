#!/bin/bash

for DRAMPOWER in 5 10 20 30 40 50 60 70 80 90 100
do
	echo "DRAM POWER = " ${DRAMPOWER} "/10 W"
	./test-3dice CONFIG/3DICE/power_${DRAMPOWER}_5_5.txt 200 0.002 > ../../../../../Data/P_T_3dice/FullStack_${DRAMPOWER}_5_5.txt
done

for SPOTPOWER in 1 2 3 4 10 15 20 25 30 35 40
do
	echo "PER BANK L2CACHE = " ${SPOTPOWER} "/10 W"
	./test-3dice CONFIG/3DICE/power_5_${SPOTPOWER}_5.txt 200 0.002 > ../../../../../Data/P_T_3dice/FullStack_5_${SPOTPOWER}_5.txt
done

for COREPOWER in 10 15 20 25 30 35 40
do
	echo "PER COREPOWER= " ${COREPOWER} "/10 W"
	./test-3dice CONFIG/3DICE/power_5_5_${COREPOWER}.txt 200 0.002 > ../../../../../Data/P_T_3dice/FullStack_5_5_${COREPOWER}.txt
done

