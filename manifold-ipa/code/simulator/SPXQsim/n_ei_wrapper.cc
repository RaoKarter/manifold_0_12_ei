/*
 * n_ei_wrapper.cc
 *
 *  Created on: Aug 16, 2017
 *      Author: karthik
 */

#include <string.h>
#include <sys/time.h>

#include "n_ei_wrapper.h"
#include "sram_perf.h"
#include "sram_volt.h"

//n_ei_wrapper.cc works only for sequential implementation right now, also assume every core starts at the same clock frequency initially

using namespace manifold::n_ei_wrapper;

int n_synced = 0;
int n_NUM_CORES;
tick_t n_SAMPLING_CYCLE;
int src1 = 0, dst1 = 1;
vector<double> ddr_bandwidth;
vector<double> ddr_bytestxed;
vector<double> ddr_power0;
vector<double> ddr_power1;
vector<double> ddr_power2;
vector<double> ddr_power3;

vector<double> n_Core_Performance;
vector< vector<double> > n_Performance;
vector<double> n_Core_Ops_Byte;
vector< vector<double> > n_Ops_Byte;

uint64_t n_reg_sam_iter;	// Counter for number of samples between two control cycles
n_ei_wrapper_t::n_ei_wrapper_t(manifold::kernel::Clock* clk, double supply_voltage, vector<manifold::spx::spx_core_t*> p_core,
		manifold::spx::pipeline_counter_t* proc_cnt, manifold::mcp_cache_namespace::L1_counter_t* c1_cnt, manifold::mcp_cache_namespace::L2_counter_t* c2_cnt,
		manifold::mcp_cache_namespace::LLP_cache* p_l1, manifold::mcp_cache_namespace::LLS_cache* p_l2,	manifold::dramsim::Dram_sim *mc,
		unsigned DRAM_freq_opt,
		double sampling_period, int num_nodes, int uid)
{
	clock = clk;
	id = uid;
	init_vdd = supply_voltage;
	char ModuleID[64];
	manifold::kernel::Clock::Register<n_ei_wrapper_t>(manifold::kernel::Clock::Master(),this,&n_ei_wrapper_t::tick,
			(void(n_ei_wrapper_t::*)(void))0);

	cerr << "Core" << id << " Vdd= " << init_vdd << " (V) Freq: " << clock->freq << "Hz" << endl << flush;

	n_SAMPLING_CYCLE = (tick_t)(sampling_period * manifold::kernel::Clock::Master().freq);

	cerr << "n_SAMPLING_CYCLE" << this->id << " " << n_SAMPLING_CYCLE << endl;
	p_cores_global = p_core;
	p_cnt = proc_cnt;
	l1_cnt = c1_cnt;
	l2_cnt = c2_cnt;
	p_l1cache = p_l1;
	p_l2cache = p_l2;
	mem_ctrl = mc;
	p_cnt->period = sampling_period;
	p_cnt->time_tick = 0;
	p_l2cache->clear_mem_counters();

	slack_cycle = 0;
	sam_cycle = 0;
	V_Old = init_vdd;
	num_samples = int(CONTROL_CYCLE/sampling_period);
	DRAM_freq_option = DRAM_freq_opt;

	switch (DRAM_freq_option)
	{
	case 1:
		cerr << "DRAM_freq_scaling_factor 1" << endl;
		break;
	case 2:
		cerr << "DRAM_freq_scaling_factor 0.83375" << endl;
		break;
	case 3:
		cerr << "DRAM_freq_scaling_factor 0.66625" << endl;
		break;
	case 4:
		cerr << "DRAM_freq_scaling_factor 0.5" << endl;
		break;
	default:
		cerr << "Wrong option. Defaulting to 1" << endl;
		DRAM_freq_option = 1;
		break;
	}

//	cerr << "n_ei_wrapper" << id << " pointing to mem_ctrl: " << hex << mem_ctrl << dec
//		 << " num samples " << num_samples << endl << flush;

//	if (id == num_nodes - 1)
//	{
//		n_NUM_CORES = num_nodes;
//		n_synced = 0;
//		swap_cores(0, 5);
//		src1 = 5;
//		dst1 = 0;
//	}
//	cerr << "Printing from Core" << id << endl;
//	for(int i = 0; i < num_nodes; i++)
//	{
//		cerr << "Core" << p_cores_global[i]->get_core_id() << "\t" ;
//	}
//	cerr << endl;

	if (id == num_nodes - 1)
	{
		ddr_bandwidth.resize(num_nodes);
		ddr_bytestxed.resize(num_nodes);
		ddr_power0.resize(num_nodes);
		ddr_power1.resize(num_nodes);
		ddr_power2.resize(num_nodes);
		ddr_power3.resize(num_nodes);

		n_reg_sam_iter = 0;
		n_Core_Performance.resize(num_nodes);
		n_Performance.resize(num_nodes, vector<double>(num_samples));
		n_Core_Ops_Byte.resize(num_nodes);
		n_Ops_Byte.resize(num_nodes, vector<double>(num_samples));
		n_NUM_CORES = num_nodes;
		Control_Cycle = 0;
	}
}

n_ei_wrapper_t::~n_ei_wrapper_t()
{

}

double n_total_mips = 0;
uint64_t n_reads[16] = {0};
uint64_t n_writes[16] = {0};


void n_ei_wrapper_t::tick()
{
	int temp_src = 0;
	sam_cycle += 1;

	if(sam_cycle == (n_SAMPLING_CYCLE + 2) )
	{
		//cerr << "sam_cycle" << this->id << "= 2" << endl << flush;
		sam_cycle = 2;
		n_total_mips = 0.0;
		if (n_reg_sam_iter == num_samples)
		{

			n_Core_Performance[id] = std::accumulate(n_Performance.at(id).begin(), n_Performance.at(id).end(), 0.0) / (n_Performance.at(id).size());
			n_Core_Ops_Byte[id] = std::accumulate(n_Ops_Byte.at(id).begin(), n_Ops_Byte.at(id).end(), 0.0) / (n_Ops_Byte.at(id).size());

			cerr << "CCCORE" << id << "\tcycle\t" << clock->NowTicks() //<< "\t" << Core_Temperature[id]
			                                                           << "\t" << n_Core_Performance[id]
			                                                           << "\t" << n_Core_Ops_Byte[id]
			                                                           << endl;
			Control_Cycle++;
			/*
			if (Control_Cycle == 50)
			{
				unsigned opt = 4;
				cerr << "@ " << clock->NowTicks() << " ei_controller asked MC to change frequency." << endl;
				mem_ctrl->changeDRAMTiming(opt);
			}
			if (Control_Cycle == 100)
			{
				unsigned opt = 2;
				cerr << "@ " << clock->NowTicks() << " ei_controller asked MC to change frequency." << endl;
				mem_ctrl->changeDRAMTiming(opt);
			}
			*/
			if (id == n_NUM_CORES - 1)
			{
				n_reg_sam_iter = 0;
				for(int i = 0; i < n_NUM_CORES; i++)
					for(int j = 0; j < num_samples; j++)
					{
						n_Performance[i][j] = 0.0;
						n_Ops_Byte[i][j] = 0.0;
					}
			}
		}
	}

	if(sam_cycle == (n_SAMPLING_CYCLE + 1))
	{
		//sam_cycle = 1;
		//n_total_mips = 0.0;
		n_synced += 1;

		unsigned long cycles = (unsigned long) (p_cnt->period * 800000000);

		switch (DRAM_freq_option)
		{
		case 1:
			cycles = (unsigned long) (p_cnt->period * 800000000); // t_CK = 1.25
			break;
		case 2:
			cycles = (unsigned long) (p_cnt->period * 667000000); // t_CK = 1.5
			break;
		case 3:
			cycles = (unsigned long) (p_cnt->period * 533000000); // t_CK = 1.87
			break;
		case 4:
			cycles = (unsigned long) (p_cnt->period * 400000000); // t_CK = 2.5
			break;
		default:
			cycles = (unsigned long) (p_cnt->period * 800000000); // t_CK = 1.25
			break;
		}

		// If frequency scaling is done, DRAM power estimates have to be scaled accordingly
		vault = mem_ctrl->getDRAMPowerBWStats(cycles, DRAM_freq_option);
		ddr_bandwidth[id] = vault->BandWidth;
		ddr_bytestxed[id] = vault->totalBytes;
		ddr_power0[id] = vault->AveragePower[0];
		ddr_power1[id] = vault->AveragePower[1];
		ddr_power2[id] = vault->AveragePower[2];
		ddr_power3[id] = vault->AveragePower[3];

		if(n_synced == n_NUM_CORES)
		{
			n_synced = 0;
			for (int i = 0; i < n_NUM_CORES; i++)
			{
				if (n_Performance[i][n_reg_sam_iter] == 0)	// If no instructions are fetched, ops/byte = 0;
				{
					n_Ops_Byte[i][n_reg_sam_iter] = 0;
				}
				else
				{
					if (n_reads[i] == 0 )
						n_Ops_Byte[i][n_reg_sam_iter] = (n_Performance[i][n_reg_sam_iter]*p_cnt->period*1e6);	// Avoiding inf
					else
						n_Ops_Byte[i][n_reg_sam_iter] = (n_Performance[i][n_reg_sam_iter]*p_cnt->period*1e6)/(n_reads[i]*64);	// Not using write backs because they don't truly reflect
																													// mem boundedness.
				}
				cerr << "Core" << i << "\tclock\t" << clock->NowTicks() << "\tMEM_READS\t" << n_reads[i] << "\tMEM_WRITES\t" << n_writes[i]
																		<< "\tOpsByte\t" << n_Ops_Byte[i][n_reg_sam_iter] << "\tBandwidth\t" << ddr_bandwidth[i]
																		<< "\tAveragePower0\t" << ddr_power0[i]
																		<< "\tAveragePower1\t" << ddr_power1[i]
																		<< "\tAveragePower2\t" << ddr_power2[i]
																		<< "\tAveragePower3\t" << ddr_power3[i]
																		<< endl << flush;
				n_reads[i] = 0;
				n_writes[i] = 0;
			}
			n_reg_sam_iter += 1;
//			if(clock->NowTicks() == 100000)
//			{
//				cerr << "%%%%%%%%%%%% SWAPPING CORE " << src1 << " WITH CORE " << dst1 << " %%%%%%%%%%%%%" << endl;
//				swap_cores(src1, dst1);
//				temp_src = src1;
//				src1 = dst1;
//				dst1 = temp_src;
//
//				cerr << "Printing from Core" << id << endl;
//				for(int i = 0; i < n_NUM_CORES; i++)
//				{
//					cerr << "Core" << p_cores_global[i]->get_core_id() << "\t" ;
//				}
//				cerr << endl;
//
//			}

		}
	}


	if(sam_cycle == n_SAMPLING_CYCLE)
	{
//		cerr << "Going to compute CPU" << this->id << " power" << endl << flush;
		char ModuleID[64];
		bool var1;
		p_cnt->time_tick += p_cnt->period; // update time_tick
		n_Performance[id][n_reg_sam_iter] = p_cnt->retire_inst.read/p_cnt->period/1e6;
		n_total_mips += p_cnt->retire_inst.read/p_cnt->period/1e6;
		cerr << "CORE" << id << "\tcycle\t" << clock->NowTicks() << "\tIPC_inst\t" << ((float)p_cnt->retire_inst.read)/n_SAMPLING_CYCLE
				             << "\tMIPS\t" << n_Performance[id][n_reg_sam_iter] << "\ttotal_MIPS\t" << n_total_mips
						     << "\tmem_reads\t" << p_l2cache->memreads<< "\tmem_writes\t"<< p_l2cache->memwrites
							 << "\tfetched_inst\t" << p_cnt->fetch_inst.read << "\tnop_inst\t" << p_cnt->nop_inst.read
							 << "\tidle_cycles\t" << p_cnt->idle_cycle.read;
							 //<< "\tclock_freq\t" << clock->freq << "\tVdd\t" << V_Old;
		mem_ctrl->PrintDRAMInstStats();
		cerr << endl;
		//var1 = mem_ctrl->CheckMCQueueStatus();
		for(int i = 0; i < n_NUM_CORES; i++)
		{
			n_reads[i] += p_l2cache->mem_reads[i];
			n_writes[i] += p_l2cache->mem_writes[i];
		}
		p_cnt->reset();
		p_l2cache->clear_my_counters();
		p_l2cache->clear_mem_counters();

	}

//	if (clock->NowTicks() == 12000000)
//	{
//		unsigned opt = 4;
//		cerr << "@ " << clock->NowTicks() << " ei_controller asked MC to change frequency." << endl;
//		mem_ctrl->changeDRAMTiming(opt);
//	}
//
//	if (clock->NowTicks() == 24000000)
//	{
//		unsigned opt = 2;
//		cerr << "@ " << clock->NowTicks() << " ei_controller asked MC to change frequency." << endl;
//		mem_ctrl->changeDRAMTiming(opt);
//	}

}




