#!/bin/bash


for SPOTPOWER in 1 1.5 2 2.5 3 3.5 4 4.5
do
	for COREPOWER in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8
	do
		echo "SPOTPOWER= " ${SPOTPOWER} " COREPOWER= " ${COREPOWER}
		./test-3dice ${SPOTPOWER} ${COREPOWER} > ../../../../../Data/P_T_3dice/Core${COREPOWER}_Cache${SPOTPOWER}.txt
 	done
	echo 
done

