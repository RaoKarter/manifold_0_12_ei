/*
 * dram_power_power.cpp
 *
 *  Created on: Jul 21, 2017
 *      Author: karthik
 */

#include "dram_print_power.h"

void dram_print(manifold::dramsim::Dram_sim* mc, int i)
{
	double test_var1 = 0.0;
	double test_var2 = 0.0;

	//DRAM Power arrays
	avgPowerBW* temp_vault = mc->getDRAMPowerBWStats();
	cerr << "Average Power Vault" << i << "\trank0\t" << temp_vault->AveragePower[0]
                                       << "\trank1\t" << temp_vault->AveragePower[1]
	                                   << "\trank2\t" << temp_vault->AveragePower[2]
	                                   << "\trank3\t" << temp_vault->AveragePower[3] << endl << flush;
	cerr << "Bg Power Vault" << i << "\trank0\t" << temp_vault->BackgroundPower[0]
		                        << "\trank1\t" << temp_vault->BackgroundPower[1]
		                        << "\trank2\t" << temp_vault->BackgroundPower[2]
		                        << "\trank3\t" << temp_vault->BackgroundPower[3] << endl << flush;
	cerr << "Refresh Power Vault" << i << "\trank0\t" << temp_vault->RefreshPower[0]
		                        << "\trank1\t" << temp_vault->RefreshPower[1]
		                        << "\trank2\t" << temp_vault->RefreshPower[2]
		                        << "\trank3\t" << temp_vault->RefreshPower[3] << endl << flush;
	cerr << "Burst Power Vault" << i <<"\trank0\t" << temp_vault->BurstPower[0]
		                        << "\trank1\t" << temp_vault->BurstPower[1]
		                        << "\trank2\t" << temp_vault->BurstPower[2]
		                        << "\trank3\t" << temp_vault->BurstPower[3] << endl << flush;
	cerr << "ActPrePower Power Vault" << i <<"\trank0\t" << temp_vault->ActPrePower[0]
		                        << "\trank1\t" << temp_vault->ActPrePower[1]
		                        << "\trank2\t" << temp_vault->ActPrePower[2]
		                        << "\trank3\t" << temp_vault->ActPrePower[3] << endl << flush;
	test_var1 = temp_vault->AveragePower[0];
	test_var2 = temp_vault->ActPrePower[1];
	cerr << "Adding values: " << test_var1 + test_var2 << " silly test again " << temp_vault->AveragePower[0] + temp_vault->ActPrePower[1] << endl;
}


