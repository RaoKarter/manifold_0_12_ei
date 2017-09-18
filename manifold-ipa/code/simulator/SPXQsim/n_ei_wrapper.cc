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


n_ei_wrapper_t::n_ei_wrapper_t(manifold::kernel::Clock* clk, double supply_voltage,
		manifold::spx::pipeline_counter_t* proc_cnt, manifold::mcp_cache_namespace::L1_counter_t* c1_cnt, manifold::mcp_cache_namespace::L2_counter_t* c2_cnt,
		manifold::mcp_cache_namespace::LLP_cache* p_l1, manifold::mcp_cache_namespace::LLS_cache* p_l2,	manifold::dramsim::Dram_sim *mc,
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
	num_samples = int(2e-1/sampling_period);

	cerr << "n_ei_wrapper" << id << " pointing to mem_ctrl: " << hex << mem_ctrl << dec
		 << " num samples " << num_samples << endl << flush;

	if (id == num_nodes - 1)
	{
		n_NUM_CORES = num_nodes;
		n_synced = 0;
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

	sam_cycle += 1;

	if(sam_cycle == (n_SAMPLING_CYCLE + 1))
	{
		sam_cycle = 1;
		n_total_mips = 0.0;
		n_synced += 1;

		if(n_synced == n_NUM_CORES)
		{
			n_synced = 0;
			for (int i = 0; i < n_NUM_CORES; i++)
			{
				cerr << "Core" << i << "\tMEM_READS\t" << n_reads[i] << "\tMEM_WRITES\t" << n_writes[i] << endl << flush;
				n_reads[i] = 0;
				n_writes[i] = 0;
			}
		}
	}


	if(sam_cycle == n_SAMPLING_CYCLE)
	{
//		cerr << "Going to compute CPU" << this->id << " power" << endl << flush;
		char ModuleID[64];

		p_cnt->time_tick += p_cnt->period; // update time_tick
		n_total_mips += p_cnt->retire_inst.read/p_cnt->period/1e6;
		cerr << "CORE" << id << "\tcycle\t" << clock->NowTicks() << "\tIPC_inst\t" << ((float)p_cnt->retire_inst.read)/n_SAMPLING_CYCLE
				             << "\tMIPS\t" << p_cnt->retire_inst.read/p_cnt->period/1e6 << "\ttotal_MIPS\t" << n_total_mips
						     << "\tmem_reads\t" << p_l2cache->memreads<< "\tmem_writes\t"<< p_l2cache->memwrites
							 << "\tfetched_inst\t" << p_cnt->fetch_inst.read << "\tnop_inst\t" << p_cnt->nop_inst.read
							 << "\tclock_freq\t" << clock->freq << "\tVdd\t" << V_Old << endl << flush;
		for(int i = 0; i < n_NUM_CORES; i++)
		{
			n_reads[i] += p_l2cache->mem_reads[i];
			n_writes[i] += p_l2cache->mem_writes[i];
		}
		p_cnt->reset();
		p_l2cache->clear_my_counters();
		p_l2cache->clear_mem_counters();

	}
}




