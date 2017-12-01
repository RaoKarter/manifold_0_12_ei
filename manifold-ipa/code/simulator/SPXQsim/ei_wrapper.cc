#include <string.h>
#include <sys/time.h>

#include "ei_wrapper.h"
#include "sram_perf.h"
#include "sram_volt.h"

//ei_wrapper.cc works only for sequential implementation right now, also assume every core starts at the same clock frequency initially

using namespace manifold::ei_wrapper;

#define EI_COMPUTE
vector<double> ad; 
vector<double> bd; 
vector<double> cd; 
vector<double> dd; 
vector<double> ed; 
vector<double> fd;
vector<double> dram0_d;
vector<double> dram1_d;
vector<double> dram2_d;
vector<double> dram3_d;

vector<double> Core_Power_l;
vector<double> Core_Power_d;
vector<double> Core_Temperature;
vector<double> Core_Performance;
vector<double> Core_Ops_Byte;
vector<double> Core_IPC;
vector<double> Core_Load;

// Global arrays to collect data
// NUM_CORES * number of samples
vector< vector<double> > TArray;
vector< vector<double> > Pd;
vector< vector<double> > Pl;
vector< vector<double> > Performance;
vector< vector<double> > Ops_Byte;
vector< vector<double> > IPCArray;
vector< vector<double> > CPU_Load;

int synced = 0;			// Variable used to figure out when to print power, temperature data.
int NUM_CORES;			// Total number of cores TODO: Should be a parameters passed or referred to by ei_wrapper
tick_t SAMPLING_CYCLE;	// Number of master clk cycles in a control interval. Since ei_wrapper subscribes to the master clock, this parameter is good.
uint64_t reg_sam_iter;	// Counter for number of samples between two control cycles
double u_k[] = {0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.05,1.1,1.15,1.2,1.25,1.3,1.35,1.4,1.45,1.5};	// Set of clock frequencies supported


ei_wrapper_t::ei_wrapper_t(manifold::kernel::Clock* clk, double supply_voltage, EI::energy_introspector_t *energy_introspector,
		manifold::spx::pipeline_counter_t* proc_cnt, manifold::spx::ipa_t* proc_ipa,
		manifold::mcp_cache_namespace::L1_counter_t* c1_cnt, manifold::mcp_cache_namespace::L2_counter_t* c2_cnt,
		manifold::mcp_cache_namespace::LLP_cache* p_l1, manifold::mcp_cache_namespace::LLS_cache* p_l2, manifold::dramsim::Dram_sim *mc,
		double tt, double sampling_period, int num_nodes, int uid)
{
	clock = clk;
	id = uid;
	init_vdd = supply_voltage;
	thermal_threshold = tt;
	char ModuleID[64];
	manifold::kernel::Clock::Register<ei_wrapper_t>(manifold::kernel::Clock::Master(),this,&ei_wrapper_t::tick,
			(void(ei_wrapper_t::*)(void))0);
	ei = energy_introspector;

	cerr << "Core" << id << " Vdd= " << init_vdd << " (V) Freq: " << clock->freq << "Hz" << endl << flush;

	sprintf(ModuleID,"CORE_DIE:FRT%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),init_vdd);
	ei->update_variable_partition(string(ModuleID),string("temperature"),300.0);

	sprintf(ModuleID,"CORE_DIE:SCH%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),init_vdd);
	ei->update_variable_partition(string(ModuleID),string("temperature"),300.0);

	sprintf(ModuleID,"CORE_DIE:INT%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),init_vdd);
	ei->update_variable_partition(string(ModuleID),string("temperature"),300.0);

	sprintf(ModuleID,"CORE_DIE:FPU%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),init_vdd);
	ei->update_variable_partition(string(ModuleID),string("temperature"),300.0);

	sprintf(ModuleID,"CORE_DIE:MEM%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),init_vdd);
	ei->update_variable_partition(string(ModuleID),string("temperature"),300.0);

	sprintf(ModuleID,"DL2_DIE:SPOT%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),init_vdd);
	ei->update_variable_partition(string(ModuleID),string("temperature"),300.0);

	SAMPLING_CYCLE = (tick_t)(sampling_period * manifold::kernel::Clock::Master().freq);

	p_cnt = proc_cnt;
	p_ipa = proc_ipa;
	l1_cnt = c1_cnt;
	l2_cnt = c2_cnt;
	p_l1cache = p_l1;
	p_l2cache = p_l2;
	mem_ctrl = mc;
	p_cnt->period = sampling_period;
	p_cnt->time_tick = 0;
	p_l2cache->clear_mem_counters();

	last_vdd = 0.8;
	slack_cycle = 0;
	num_cycles_old = 0;

	sam_cycle = 0;
	V_Old = init_vdd;
	num_samples = int(CONTROL_CYCLE/sampling_period);	// Num of samples between subsequent control cycles

#if 0
//  Temperature regulation parameter init
//	N1 = 0.1875e9; 		// For a total of 3billion transistors for all 16 cores
//	k_design = 5.0;
//	I_s0 = 5e-7;
//	Beta1 = N1*k_design*I_s0;                                // Static power constant
//	Gamma1 = (-q1*(V_off + threshold_voltage)) / (2.303*eta*kb);      // Static power constant
//	g = 33;
//	StepSize = 1;
//
#endif

//	cerr << "ei_wrapper" << id << " pointing to mem_ctrl: " << hex << mem_ctrl << dec
//		 << " num samples " << num_samples << endl << flush;

	// Initializing controller parameters
	Core_Total_Power = 0.0;
	ALPHA = 0.0;
	Performance_Slope = 1;
	R_P = 1e-6;
	Core_Total_Power_Estimate.resize(NUM_AVAIL_FREQS);
	Core_Temperature_Estimate.resize(NUM_AVAIL_FREQS);
	Z.resize(NUM_AVAIL_FREQS);
	Temperature_Cost.resize(NUM_AVAIL_FREQS);
	Performance_Cost.resize(NUM_AVAIL_FREQS);
	Total_Cost.resize(NUM_AVAIL_FREQS);
	Control_Cycle = 0;
	Control_Sat_Counter = 0;

	if (id == num_nodes - 1)
	{
		NUM_CORES = num_nodes;
		synced = 0;


		ad.resize(num_nodes);
		bd.resize(num_nodes);
		cd.resize(num_nodes);
		dd.resize(num_nodes);
		ed.resize(num_nodes);
		fd.resize(num_nodes);
		dram0_d.resize(num_nodes);
		dram1_d.resize(num_nodes);
		dram2_d.resize(num_nodes);
		dram3_d.resize(num_nodes);

		Core_Temperature.resize(num_nodes);
		Core_Performance.resize(num_nodes);
		Core_Ops_Byte.resize(num_nodes);
		Core_Power_d.resize(num_nodes);
		Core_Power_l.resize(num_nodes);
		Core_IPC.resize(num_nodes);
		Core_Load.resize(num_nodes);

		reg_sam_iter = 0;

		TArray.resize(num_nodes, vector<double>(num_samples));
		Pd.resize(num_nodes, vector<double>(num_samples));
		Pl.resize(num_nodes, vector<double>(num_samples));
		Performance.resize(num_nodes, vector<double>(num_samples));
		Ops_Byte.resize(num_nodes, vector<double>(num_samples));
		IPCArray.resize(num_nodes, vector<double>(num_samples));
		CPU_Load.resize(num_nodes, vector<double>(num_samples));
		cerr << "SAMPLING_CYCLE" << this->id << " " << SAMPLING_CYCLE << " CONTROL_CYCLE " << CONTROL_CYCLE << endl;
	}

}

ei_wrapper_t::~ei_wrapper_t()
{

}
// Global variables used when measuring power, energy and temperature
double core_leakage = 0;
double sram_leakage = 0;
double dram_leakage_energy = 0;
double total_leakage = 0;
double core_energy = 0;
double sram_energy = 0;
double dram_energy = 0;
double total_energy = 0;
double factor = 5;
double factor2 = 0.64;
double factor3 = 2.65;
double dram_factor = 2;
double total_mips = 0;
uint64_t reads[16] = {0};
uint64_t writes[16] = {0};

double ei_wrapper_t::ComputeR_PRange2(double R_i, bool f)
{
	int iter1 = 0;
	vector<double>Perf_Cost1, J1;
	double d_first = 0.0;
	double d_last = 0.0;

	Perf_Cost1.resize(NUM_AVAIL_FREQS);
	J1.resize(NUM_AVAIL_FREQS);

	if (f == false)
	{
		cerr << "Range2 Increase R_P" << endl;
		while (1)
		{
			for (int i = 0; i < NUM_AVAIL_FREQS; i++)
			{
				Perf_Cost1[i] = R_i * Performance_Slope * u_k[i] * 1e3;
				J1[i] = Temperature_Cost[i] + Perf_Cost1[i];
			}
			d_first = J1[1] - J1[0];
			d_last = J1[NUM_AVAIL_FREQS-1] - J1[NUM_AVAIL_FREQS-2];
			R_i += 1e-6;
			iter1 += 1;
			if (d_first > 0 && d_last > 0)
			{
				R_i = R_i - 1e-6;
				if (R_i < 0)
					R_i = 1e-6;
				break;
			}
		}
	}
	else
	{
		cerr << "Range2 Decrease R_P" << endl;
		while (1)
		{
			for (int i = 0; i < NUM_AVAIL_FREQS; i++)
			{
				Perf_Cost1[i] = R_i * Performance_Slope * u_k[i] * 1e3;
				J1[i] = Temperature_Cost[i] + Perf_Cost1[i];
			}
			d_first = J1[1] - J1[0];
			d_last = J1[NUM_AVAIL_FREQS-1] - J1[NUM_AVAIL_FREQS-2];
			R_i -= 1e-6;
			// R_i HAS to be positive
			if(R_i < 0)
			{
				R_i = 1e-6;
				break;
			}
			iter1 += 1;
			if (d_first < 0 && d_last < 0)
			{
				R_i = R_i + 1e-6;
				break;
			}
		}
	}
	cerr << "Range2 iter " << iter1 << " R_i " << R_i << endl;
	return R_i;
}

void ei_wrapper_t::ComputeR_PRange1(R_PRange* R_hat, double R_i)
{
	vector<double>Perf_Cost1, TC;
	double d_first = Total_Cost[1] - Total_Cost[0];
	double d_last = Total_Cost[NUM_AVAIL_FREQS - 1] - Total_Cost[NUM_AVAIL_FREQS - 2];
	bool flag;
	int iter = 0;
	double R_P1,R_P2;

	R_P1 = R_i;

	cerr << "Range1 d_first " << d_first << " d_last " << d_last << " R_P1 " << R_P1 << " PS " << Performance_Slope << endl;

	Perf_Cost1.resize(NUM_AVAIL_FREQS);
	TC.resize(NUM_AVAIL_FREQS);

	while (1)
	{
		if (d_first < 0 && d_last < 0)
		{
			// Need to increase R_P so that we reach the point of concavity
			flag = false;
			for (int i = 0; i < NUM_AVAIL_FREQS; i++)
			{
				Perf_Cost1[i] = R_P1 * Performance_Slope * u_k[i] * 1e3;
			}
			R_P1 += 1e-6;
		}
		else if (d_first > 0 && d_last > 0)
		{
			// Need to decrease R_O so that we reach the point of concavity
			flag = true;
			for (int i = 0; i < NUM_AVAIL_FREQS; i++)
			{
				Perf_Cost1[i] = R_P1 * Performance_Slope * u_k[i] * 1e3;
			}
			R_P1 -= 1e-6;
			if (R_P1 < 0)
			{
				R_P1 = 1e-6;
				break;
			}
		}
		else
		{
			// Already concave
			cerr << "Concavity Reached iter_init " << iter << " ";
			if ((d_first > 0 && d_last < 0) && (iter == 0) && Total_Cost[NUM_AVAIL_FREQS-1] > Total_Cost[0])
			{
				cerr << "Range1 increasing R_P to reach R_max ";
				while(d_first > 0 && d_last < 0)
				{
					for (int i = 0; i < NUM_AVAIL_FREQS; i++)
					{
						Perf_Cost1[i] = R_P1 * Performance_Slope * u_k[i] * 1e3;
						TC[i] = Temperature_Cost[i] + Perf_Cost1[i];
					}

					R_P1 += 1e-6;
					d_first = TC[1] - TC[0];
					d_last = TC[NUM_AVAIL_FREQS - 1] - TC[NUM_AVAIL_FREQS - 2];
					iter += 1;
				}
				flag = true;
			}
			else if ((d_first > 0 && d_last < 0) && (iter == 0) && Total_Cost[NUM_AVAIL_FREQS-1] < Total_Cost[0])
			{
				cerr << "Range1 decreasing R_P to reach R_min ";
				while(d_first > 0 && d_last < 0)
				{
					for (int i = 0; i < NUM_AVAIL_FREQS; i++)
					{
						Perf_Cost1[i] = R_P1 * Performance_Slope * u_k[i] * 1e3;
						TC[i] = Temperature_Cost[i] + Perf_Cost1[i];
					}

					R_P1 -= 1e-6;
					if (R_P1 < 0)
					{
						R_P1 = 1e-6;
						break;
					}
					d_first = TC[1] - TC[0];
					d_last = TC[NUM_AVAIL_FREQS - 1] - TC[NUM_AVAIL_FREQS - 2];
					iter += 1;
				}
				flag = false;
			}
			break;
		}

		for(int i = 0; i < NUM_AVAIL_FREQS; i++)
		{
			TC[i] = Temperature_Cost[i] + Perf_Cost1[i];
		}
		d_first = TC[1] - TC[0];
		d_last = TC[NUM_AVAIL_FREQS - 1] - TC[NUM_AVAIL_FREQS - 2];
		iter += 1;
	}

	cerr << "Iter_final " << iter << endl;
	R_P2 = ComputeR_PRange2(R_P1,flag);
	if (R_P1 < R_P2)
	{
		R_hat->R_P_min = R_P1;
		R_hat->R_P_max = R_P2;
	}
	else
	{
		R_hat->R_P_min = R_P2;
		R_hat->R_P_max = R_P1;
	}
	return;
}

void ei_wrapper_t::ApplyOndemand()
{
#ifdef C_ENABLED
	// IF fetched_inst = 0
	//		Load = 0
	// ELSE
	// 		Load = ( nop_inst / fetched_inst ) * 100
	// IF Load > 80% then increase CPU freq to max.
	// ELSEIF Load == 0 decrease freq step by step
	// ELSE freq = freq_min + load * (f_max - f_min) / 100

	double u_opt = 0.0;
	double V_new = 0.0;

	phi_Old = double (clock->freq / 1e9 ); // Freq in GHz
	cerr << "%%%%%%%%%%%%% CORE " << id << " BEGIN %%%%%%%%%%%%%%%%%" << endl << endl;
	if (Core_Load[id] == 0)
	{
		u_opt = phi_Old - 0.5;
		if (u_opt <= 0.5)
			u_opt = 0.5;
	}
	else if (Core_Load[id] >= 0.8)
		u_opt = 1.5;
	else
	{
		u_opt = 0.5 + Core_Load[id];
		if(u_opt >= 1.5)
			u_opt = 1.5;
	}

	cerr << "CC\t" << Control_Cycle << "\tCCCore" << id << "\tLoad\t" << Core_Load[id] << "\tu_opt\t" << u_opt << endl;

	cerr << "%%%%%%%%%%%%% CORE " << id << "END %%%%%%%%%%%%%%%%%%%" << endl << endl;


	// Assign u_opt to the core and the associated L2-Cache
	V_new = 0.8 + 0.1*(u_opt - 3);
	clock->set_frequency(double(u_opt*1e9));
//		cerr << "Changed CPU Clock and L2 Cache Clock" << endl;
	p_l1cache->m_clock->set_frequency(double(u_opt*1e9));
//		cerr << "Changed L1 Cache Clock" << endl;
//		cerr << "\tphi_applied\t" << u_opt << endl;

	char ModuleID[64];

	sprintf(ModuleID,"CORE_DIE:FRT%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

	sprintf(ModuleID,"CORE_DIE:SCH%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

	sprintf(ModuleID,"CORE_DIE:INT%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

	sprintf(ModuleID,"CORE_DIE:FPU%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

	sprintf(ModuleID,"CORE_DIE:MEM%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

	sprintf(ModuleID,"DL2_DIE:SPOT%d",id);
	ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);


	phi_Old = u_opt;
	V_Old = V_new;
	Control_Cycle += 1;
#endif
}

void ei_wrapper_t::ApplyControl()
{
#ifdef C_ENABLED

		std::vector<double>::iterator max_index;
		double u_opt = 0.0;
		double V_new = 0.0;
		R_PRange R_hat;
		double R_P_local = 0.0;
		double delta_R_P = 0.0;
		R_hat.R_P_min = 1e-6;
		R_hat.R_P_max = 1e-6;

		phi_Old = double (clock->freq / 1e9 ); // Freq in GHz
		cerr << "%%%%%%%%%%%%% CORE " << id << " BEGIN %%%%%%%%%%%%%%%%%" << endl << endl;

		if (Core_Temperature[id] > T_MAX)
		{
			// Reduce temperature ASAP
			u_opt = u_k[0];
			cerr << "CC\t" << Control_Cycle << "\tCCCore" << id << "\tTTC R_P\t0\tu_opt\t" << u_opt << endl;

		}
		// If Temperature is very close to the threshold assign very low value to R_hat
		else if( (T_MAX - Core_Temperature[id]) < 2 )	// [T_MAX-2, T_MAX+2]
		{
			// Estimate ALPHA given static power and total power
			Core_Total_Power = Core_Power_d[id] + Core_Power_l[id];
			Performance_Slope = double(Core_Performance[id] / (phi_Old*1e3));
			if(Performance_Slope == 0)
			{
				ALPHA = 0.0;
				R_P_local = 0;
				u_opt = u_k[0];
			}
			else
			{
				ALPHA = Core_Power_d[id]/pow(phi_Old,3);

				cerr << "1Core_Power\t" << Core_Total_Power << "\tCore_Performance\t" << Core_Performance[id] << "\tALPHA\t" << ALPHA << "\tPS\t" << Performance_Slope << endl;
				cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
				for(int i = 0; i < NUM_AVAIL_FREQS; i++)
				{
					Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
					Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

					Z[i] = T_MAX - Core_Temperature_Estimate[i];
					Temperature_Cost[i] = Z[i]*Z[i];
					Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
					Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
					fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
				}
				cerr << endl;
				// Need to give highest importance to temperature now...
				ComputeR_PRange1(&R_hat, 1e-6);
				cerr << "R_P_min: " << R_hat.R_P_min << " R_P_max: " << R_hat.R_P_max << endl;
				delta_R_P = R_hat.R_P_max - R_hat.R_P_min;
				if (delta_R_P == 0)
					R_P = R_hat.R_P_min;
				else
					R_P = R_hat.R_P_min + sqrt(Core_IPC[id]/IPC_MAX)*delta_R_P;

//				R_P = (R_hat.R_P_max + R_hat.R_P_min) / 2;
				cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
				for(int i = 0; i < NUM_AVAIL_FREQS; i++)
				{
					Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
					Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

					Z[i] = T_MAX - Core_Temperature_Estimate[i];
					Temperature_Cost[i] = Z[i]*Z[i];

					Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
					Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
					fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
				}
				cerr << endl;

				R_HAT.R_P_min = R_hat.R_P_min;
				R_HAT.R_P_max = R_hat.R_P_max;
				R_P_local = R_P;

				max_index = std::max_element(Total_Cost.begin(), Total_Cost.end());
				u_opt = u_k[ std::distance(Total_Cost.begin(), max_index) ];
				cerr << "u_k[" << std::distance(Total_Cost.begin(), max_index) << "]= " << u_k[ std::distance(Total_Cost.begin(), max_index) ]
				     << " R_P " << R_P << endl;
			}

			cerr << "CC\t" << Control_Cycle << "\tCCCore" << id << "\tHT R_P\t" << R_P_local << "\tu_opt\t" << u_opt
				 << "\tR_P_min\t" << R_HAT.R_P_min << "\tR_P_max\t" << R_HAT.R_P_max << endl;
		}
		else
		{
			// If Temperature is not that close, follow the normal procedure
			// If first control cycle then estimate R_P and proceed
			if (Control_Cycle == 0)
			{
				// Estimate ALPHA given static power and total power
				Core_Total_Power = Core_Power_d[id] + Core_Power_l[id];
				Performance_Slope = double(Core_Performance[id] / (phi_Old * 1e3));
				if(Performance_Slope == 0)
				{
					ALPHA = 0.0;
					R_P_local = 0;
					u_opt = u_k[0];
				}
				else
				{
					ALPHA = Core_Power_d[id]/pow(phi_Old,3);

					cerr << "2Core_Power " << Core_Total_Power << "\tCore_Performance\t" << Core_Performance[id] << "\tALPHA\t" << ALPHA << "\tPS\t" << Performance_Slope << endl;
					cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
					for(int i = 0; i < NUM_AVAIL_FREQS; i++)
					{
						Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
						Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

						Z[i] = T_MAX - Core_Temperature_Estimate[i];
						Temperature_Cost[i] = Z[i]*Z[i];
						Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
						Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
						fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
					}
					cerr << endl;

					ComputeR_PRange1(&R_hat, R_P);
					cerr << "R_P_min: " << R_hat.R_P_min << " R_P_max: " << R_hat.R_P_max << endl;
					delta_R_P = R_hat.R_P_max - R_hat.R_P_min;
					if (delta_R_P == 0)
						R_P = R_hat.R_P_min;
					else
						R_P = R_hat.R_P_min + sqrt(Core_IPC[id]/IPC_MAX)*delta_R_P;

					cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
					for(int i = 0; i < NUM_AVAIL_FREQS; i++)
					{
						Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
						Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

						Z[i] = T_MAX - Core_Temperature_Estimate[i];
						Temperature_Cost[i] = Z[i]*Z[i];
						Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
						Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
						fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
					}
					cerr << endl;

					R_HAT.R_P_min = R_hat.R_P_min;
					R_HAT.R_P_max = R_hat.R_P_max;
					R_P_local = R_P;

					max_index = std::max_element(Total_Cost.begin(), Total_Cost.end());
					u_opt = u_k[ std::distance(Total_Cost.begin(), max_index) ];
					cerr << "u_k[" << std::distance(Total_Cost.begin(), max_index) << "]= " << u_k[ std::distance(Total_Cost.begin(), max_index) ]
									     << " R_P " << R_P << endl;
				}
			}
			else
			{
				// Some logic to kick u_opt Out of saturation
				// If u_opt is stuck at fmin or fmax for more than X control cycles
				// recalculate R_P range and assign a value R_P such that weightage
				// given to both temperature and performance is appropriate
				if ( (Control_Cycle % SATURATION_LIMIT)  == 0 )
				{
					cerr << "SATURATION_LIMIT_CYCLE " << Control_Cycle << endl;

					// Estimate ALPHA given static power and total power
					Core_Total_Power = Core_Power_d[id] + Core_Power_l[id];
					Performance_Slope = double(Core_Performance[id] / (phi_Old * 1e3));

					if(Performance_Slope == 0)
					{
						ALPHA = 0.0;
						R_P_local = 0;
						u_opt = u_k[0];
					}
					else
					{

						ALPHA = Core_Power_d[id]/pow(phi_Old,3);

						cerr << "3Core_Power " << Core_Total_Power << "\tCore_Performance\t" << Core_Performance[id] << "\tALPHA\t" << ALPHA << "\tPS\t" << Performance_Slope << endl;
						cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
						for(int i = 0; i < NUM_AVAIL_FREQS; i++)
						{
							Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
							Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

							Z[i] = T_MAX - Core_Temperature_Estimate[i];
							Temperature_Cost[i] = Z[i]*Z[i];
							Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
							Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
							fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
						}
						cerr << endl;

						ComputeR_PRange1(&R_hat, R_P);
						cerr << "R_P_min: " << R_hat.R_P_min << " R_P_max: " << R_hat.R_P_max << endl;
						delta_R_P = R_hat.R_P_max - R_hat.R_P_min;
						if (delta_R_P == 0)
							R_P = R_hat.R_P_min;
						else
							R_P = R_hat.R_P_min + sqrt(Core_IPC[id]/IPC_MAX)*delta_R_P;

						cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
						for(int i = 0; i < NUM_AVAIL_FREQS; i++)
						{
							Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
							Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

							Z[i] = T_MAX - Core_Temperature_Estimate[i];
							Temperature_Cost[i] = Z[i]*Z[i];
							Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
							Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
							fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
						}
						cerr << endl;

						R_HAT.R_P_min = R_hat.R_P_min;
						R_HAT.R_P_max = R_hat.R_P_max;
						R_P_local = R_P;

						// Find u_k for which Total_Cost is maximized
						max_index = std::max_element(Total_Cost.begin(), Total_Cost.end());
						u_opt = u_k[ std::distance(Total_Cost.begin(), max_index) ];
						cerr << "u_k[" << std::distance(Total_Cost.begin(), max_index) << "]= " << u_k[ std::distance(Total_Cost.begin(), max_index) ]
										     << " R_P " << R_P << endl;
					}
				}
				else
				{
					// Estimate ALPHA given static power and total power
					Core_Total_Power = Core_Power_d[id] + Core_Power_l[id];
					Performance_Slope = double(Core_Performance[id] / (phi_Old * 1e3));
					if(Performance_Slope == 0)
					{
						ALPHA = 0.0;
						R_P_local = 0;
						u_opt = u_k[0];
					}
					else
					{
						ALPHA = Core_Power_d[id]/pow(phi_Old,3);

						cerr << "5Core_Power " << Core_Total_Power << "\tCore_Performance\t" << Core_Performance[id] << "\tALPHA\t" << ALPHA << "\tPS\t" << Performance_Slope << endl;
						cerr << "Core_Total_Power_Estimate Core_Temperature_Estimate Temperature_Cost Performance_Cost Total_Cost" << endl;
						for(int i = 0; i < NUM_AVAIL_FREQS; i++)
						{
							Core_Total_Power_Estimate[i] = (ALPHA*pow(u_k[i],3)) + (BETA*u_k[i]) + (GAMMA*Core_Temperature[id]) + (DELTA*u_k[i]*Core_Temperature[id]) + EPSILON;
							Core_Temperature_Estimate[i] = A1*Core_Temperature[id] + (CONTROL_CYCLE)*(-B1*Core_Total_Power_Estimate[i] - C1) + 0.7;

							Z[i] = T_MAX - Core_Temperature_Estimate[i];
							Temperature_Cost[i] = Z[i]*Z[i];
							Performance_Cost[i] = R_P*Performance_Slope*u_k[i]*1e3;
							Total_Cost[i] = Temperature_Cost[i] + Performance_Cost[i];
							fprintf(stderr, "%lf\t%lf\t%lf\t%lf\t%lf\n", Core_Total_Power_Estimate[i], Core_Temperature_Estimate[i], Temperature_Cost[i], Performance_Cost[i],Total_Cost[i] );
						}
						cerr << endl;

						R_P_local = R_P;
						// Find u_k for which Total_Cost is maximized
						max_index = std::max_element(Total_Cost.begin(), Total_Cost.end());
						u_opt = u_k[ std::distance(Total_Cost.begin(), max_index) ];
						cerr << "u_k[" << std::distance(Total_Cost.begin(), max_index) << "]= " << u_k[ std::distance(Total_Cost.begin(), max_index) ]
										     << " R_P " << R_P << endl;
					}
				}
			}

			cerr << "CC\t" << Control_Cycle << "\tCCCore" << id << "\tNT R_P\t" << R_P_local << "\tu_opt\t" << u_opt
				 << "\tR_P_min\t" << R_HAT.R_P_min << "\tR_P_max\t" << R_HAT.R_P_max << endl;
		}
		cerr << "%%%%%%%%%%%%% CORE " << id << "END %%%%%%%%%%%%%%%%%%%" << endl << endl;


		// Assign u_opt to the core and the associated L2-Cache
		V_new = 0.8 + 0.1*(u_opt - 3);
		clock->set_frequency(double(u_opt*1e9));
//		cerr << "Changed CPU Clock and L2 Cache Clock" << endl;
		p_l1cache->m_clock->set_frequency(double(u_opt*1e9));
//		cerr << "Changed L1 Cache Clock" << endl;
//		cerr << "\tphi_applied\t" << u_opt << endl;

		char ModuleID[64];

		sprintf(ModuleID,"CORE_DIE:FRT%d",id);
		ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

		sprintf(ModuleID,"CORE_DIE:SCH%d",id);
		ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

		sprintf(ModuleID,"CORE_DIE:INT%d",id);
		ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

		sprintf(ModuleID,"CORE_DIE:FPU%d",id);
		ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

		sprintf(ModuleID,"CORE_DIE:MEM%d",id);
		ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);

		sprintf(ModuleID,"DL2_DIE:SPOT%d",id);
		ei->update_variable_partition(string(ModuleID),string("vdd"),V_new);


		phi_Old = u_opt;
		V_Old = V_new;
		Control_Cycle += 1;
#endif
}

void ei_wrapper_t::tick()
{

	sam_cycle += 1;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
	if(sam_cycle == (SAMPLING_CYCLE + 2) )
	{
//		cerr << "sam_cycle" << this->id << "= 2" << endl << flush;
		sam_cycle = 2;
		total_mips = 0.0;
		if (reg_sam_iter == num_samples)
		{

			Core_Temperature[id] = TArray[id][num_samples-1];
			Core_Power_d[id] = std::accumulate(Pd.at(id).begin(), Pd.at(id).end(), 0.0) / (Pd.at(id).size());
			Core_Power_l[id] = std::accumulate(Pl.at(id).begin(), Pl.at(id).end(), 0.0) / (Pl.at(id).size());
			Core_Performance[id] = std::accumulate(Performance.at(id).begin(), Performance.at(id).end(), 0.0) / (Performance.at(id).size());
			Core_Ops_Byte[id] = std::accumulate(Ops_Byte.at(id).begin(), Ops_Byte.at(id).end(), 0.0) / (Ops_Byte.at(id).size());
			Core_IPC[id] = std::accumulate(IPCArray.at(id).begin(), IPCArray.at(id).end(), 0.0) / (IPCArray.at(id).size());
			Core_Load[id] = std::accumulate(CPU_Load.at(id).begin(), CPU_Load.at(id).end(), 0.0) / (CPU_Load.at(id).size());

			cerr << "\nCCCORE" << id << "\tcycle\t" << clock->NowTicks() << "\t" << Core_Temperature[id]
			                                                           << "\t" << Core_Power_d[id]
			                                                           << "\t" << Core_Power_l[id]
			                                                           << "\t" << Core_Performance[id]
			                                                           << "\t" << Core_Ops_Byte[id]
			                                                           << "\t" << Core_IPC[id]
			                                                           << "\t" << Core_Load[id] << endl;
			//ApplyControl();

			ApplyOndemand();


			if (id == NUM_CORES - 1)
			{
				reg_sam_iter = 0;
				for(int i = 0; i < NUM_CORES; i++)
					for(int j = 0; j < num_samples; j++)
					{
						TArray[i][j] = 0.0;
						Pd[i][j] = 0.0;
						Pl[i][j] = 0.0;
						Performance[i][j] = 0.0;
						Ops_Byte[i][j] = 0.0;
						IPCArray[i][j] = 0.0;
						CPU_Load[i][j] = 0.0;
					}
			}
		}
	}

#if 1
	if(sam_cycle == (SAMPLING_CYCLE + 1))
	{
#ifdef EI_COMPUTE
		EI::power_t DRAM_P; // Temporary variable to push_data<power_t>
		char ModuleID[64];
		clock->SetNowTime(p_cnt->time_tick);
		double test_temp = 0.0;

		//***Nehalem:Memory***
		//core:DTLB
		sprintf(ModuleID,"core:DTLB:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l1_cnt->DTLB * factor);
		//core:DL1
		sprintf(ModuleID,"core:DL1:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l1_cnt->DL1 * factor);

		//core:DL2
		sprintf(ModuleID,"core:DL2:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l2_cnt->DL2 * 2.2 * factor);

		//core:DL1:MissBuffer
		sprintf(ModuleID,"core:DL1:MissBuffer:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l1_cnt->DL1.missbuf * factor);

		//core:DL1:FillBuffer
		sprintf(ModuleID,"core:DL1:FillBuffer:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l1_cnt->DL1.linefill * factor);

		//core:DL1:PrefetchBuffer
		sprintf(ModuleID,"core:DL1:PrefetchBuffer:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l1_cnt->DL1.prefetch * factor);

		//core:DL1:WBB
		sprintf(ModuleID,"core:DL1:WBB:%d",id);
		ei->compute_power(p_cnt->time_tick, p_cnt->period,string(ModuleID),l1_cnt->DL1.writeback * factor);

		l1_cnt->reset();
		l2_cnt->reset();

		//***DRAMSIM2 Power***
		unsigned long cycles = (unsigned long) (p_cnt->period * 800000000); // Assuming 800MHz DRAM Clk at t_CK = 1.25 TODO: Make this a user defined parameter
		vault = mem_ctrl->getDRAMPowerBWStats(cycles);

		// dram_factor is a crude way of estimating DRAM power. The current values are for an 8GB DRAM.
		// We therefore scale the total power by 1/2. With appropriate current values, dram_factor = 1
		DRAM_P.leakage = (vault->BackgroundPower[0] + vault->RefreshPower[0])/dram_factor;
		DRAM_P.total = vault->AveragePower[0]/dram_factor;
		dram0_d[id] = (vault->ActPrePower[0] + vault->BurstPower[0])/dram_factor;
		sprintf(ModuleID,"DRAM0_DIE:VAULT%d",id);
		ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),DRAM_P);

		DRAM_P.leakage = (vault->BackgroundPower[1] + vault->RefreshPower[1])/dram_factor;
		DRAM_P.total = vault->AveragePower[1]/dram_factor;
		dram1_d[id] = (vault->ActPrePower[1] + vault->BurstPower[1])/dram_factor;
		sprintf(ModuleID,"DRAM1_DIE:VAULT%d",id);
		ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),DRAM_P);

		DRAM_P.leakage = (vault->BackgroundPower[2] + vault->RefreshPower[2])/dram_factor;
		DRAM_P.total = vault->AveragePower[2]/dram_factor;
		dram2_d[id] = (vault->ActPrePower[2] + vault->BurstPower[2])/dram_factor;
		sprintf(ModuleID,"DRAM2_DIE:VAULT%d",id);
		ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),DRAM_P);

		DRAM_P.leakage = (vault->BackgroundPower[3] + vault->RefreshPower[3])/dram_factor;
		DRAM_P.total = vault->AveragePower[3]/dram_factor;
		dram3_d[id] = (vault->ActPrePower[3] + vault->BurstPower[3])/dram_factor;
		sprintf(ModuleID,"DRAM3_DIE:VAULT%d",id);
		ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),DRAM_P);

		synced += 1;
	
		if(synced == NUM_CORES)
		{

			synced = 0;

			double ip = 0.0;	// Total instantaneous power
			double il = 0.0;	// Total instantaneous leakage

			double ipp = 0.0;	// Total instantaneous CORE power
			double ilp = 0.0;	// Total instantaneous CORE leakage power

			double ips = 0.0;	// Total instantaneous L2CACHE power
			double ils = 0.0;	// Total instantaneous L2CACHE leakage power

			double inst_dram_p = 0.0;	// Total instantaneous DRAM power
			double inst_dram_l = 0.0;	// Total instantaneous DRAM leakage power

			for(int i=0;i<NUM_CORES;i++)
			{
				EI::power_t P;

				sprintf(ModuleID,"CORE_DIE:FRT%d",i);
				P = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				ad[i] = P.get_total() - P.leakage;
				P.leakage = P.leakage * factor2;
				P.total = P.leakage + ad[i];
				ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),P);

				sprintf(ModuleID,"CORE_DIE:SCH%d",i);
				P = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				bd[i] = P.get_total() - P.leakage;
				P.leakage = P.leakage * factor2;
				P.total = P.leakage + bd[i];
				ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),P);

				sprintf(ModuleID,"CORE_DIE:INT%d",i);
				P = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				cd[i] = P.get_total() - P.leakage;
				P.leakage = P.leakage * factor2;
				P.total = P.leakage + cd[i];
				ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),P);

				sprintf(ModuleID,"CORE_DIE:FPU%d",i);
				P = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				dd[i] = P.get_total() - P.leakage;
				P.leakage = P.leakage * factor2;
				P.total = P.leakage + dd[i];
				ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),P);

				sprintf(ModuleID,"CORE_DIE:MEM%d",i);
				P = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				ed[i] = P.get_total() - P.leakage;
				P.leakage = P.leakage * factor2;
				P.total = P.leakage + ed[i];
				ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),P);

				sprintf(ModuleID,"DL2_DIE:SPOT%d",i);
				P = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				fd[i] = P.get_total() - P.leakage;
				P.leakage = P.leakage / factor3;
				P.total = P.leakage + fd[i];
				ei->push_data<EI::power_t>(p_cnt->time_tick,p_cnt->period,string("partition"),string(ModuleID),string("power"),P);
			}

			double L2CACHE_T;
			vector<double> CORE_T;
			vector<double> DRAM_T;
			double core_pd, core_pl, core_t;
			ei->compute_temperature(p_cnt->time_tick, p_cnt->period, "16CORE");


			CORE_T.resize(5);
			DRAM_T.resize(4);

			for(int i=0;i<NUM_CORES;i++)
			{
				double s = 0.0; //accumulator for core_component total power
				double t = 0.0; //accumulator for core_component leakage power
				double dram_pt = 0.0;	//accumulator for DRAM vault total power
				double dram_pl = 0.0;	//accumulator for DRAM vault leakage power
				double a,b,c,d,e,f; //Total power of components. a,b,c,d,e refer to CORE. f is for L2CACHE
				double al, bl, cl, dl, el, fl; //Leakage power of components. al,bl,cl,dl,el refer to CORE. fl is for L2CACHE
				vector<double> dram_leak_power;

				EI::power_t update_p; // temporary variable
				dram_leak_power.reserve(4); // temporary variable to display leakage power

				// SPX Core Temperature
				sprintf(ModuleID,"CORE_DIE:FRT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				al = update_p.leakage * factor2; // update FRT leakage power
				a = al + ad[i]; // update FRT total power
				s += a;		// Accumulate total power
				t += al;	// Accumulate leakage power
				CORE_T[0] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"CORE_DIE:SCH%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				bl = update_p.leakage * factor2;
				b = bl + bd[i];
				s += b;
				t += bl;
				CORE_T[1] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"CORE_DIE:INT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				cl = update_p.leakage * factor2;
				c = cl + cd[i];
				s += c;
				t += cl;
				CORE_T[2] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"CORE_DIE:FPU%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				dl = update_p.leakage * factor2;
				d = dl + dd[i];
				s += d;
				t += dl;
				CORE_T[3] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"CORE_DIE:MEM%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				el = update_p.leakage * factor2;
				e = el + ed[i];
				s += e;
				t += el;
				CORE_T[4] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));



				// L2 Cache Temperature
				sprintf(ModuleID,"DL2_DIE:SPOT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				fl = update_p.leakage / factor3;
				f = fl + fd[i];
				L2CACHE_T = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				// DRAMSim2 Temperature
				sprintf(ModuleID,"DRAM0_DIE:VAULT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				dram_pt += update_p.total;
				dram_pl += update_p.leakage;
				dram_leak_power[0] = update_p.leakage;
				DRAM_T[0] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"DRAM1_DIE:VAULT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				dram_pt += update_p.total;
				dram_pl += update_p.leakage;
				dram_leak_power[1] = update_p.leakage;
				DRAM_T[1] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"DRAM2_DIE:VAULT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				dram_pt += update_p.total;
				dram_pl += update_p.leakage;
				dram_leak_power[2] = update_p.leakage;
				DRAM_T[2] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				sprintf(ModuleID,"DRAM3_DIE:VAULT%d",i);
				update_p = ei->pull_data<EI::power_t>(p_cnt->time_tick,string("partition"),string(ModuleID),string("power"));
				dram_pt += update_p.total;
				dram_pl += update_p.leakage;
				dram_leak_power[3] = update_p.leakage;
				DRAM_T[3] = ei->pull_data<double>(p_cnt->time_tick,string("partition"),string(ModuleID),string("temperature"));

				total_energy += (s + f + dram_pt) * p_cnt->period;
				core_energy += s * p_cnt->period;
				sram_energy += f * p_cnt->period;
				dram_energy += dram_pt * p_cnt->period;

				total_leakage += (t + fl + dram_pl) * p_cnt->period;
				core_leakage += t * p_cnt->period;
				sram_leakage += fl * p_cnt->period;
				dram_leakage_energy += dram_pl * p_cnt->period;

				// Accumulate system inst power
				ip += s + f + dram_pt;
				il += t + fl + dram_pl;
				// Accumulate CORE inst power
				ipp += s;
				ilp += t;
				// Accumulate L2CACHE inst power
				ips += f;
				ils += fl;
				// Accumulate DRAM inst power
				inst_dram_p += dram_pt;
				inst_dram_l += dram_pl;

				core_t = std::accumulate(CORE_T.begin(), CORE_T.end(), 0.0) / CORE_T.size();	// Spatial average temperature of the core
				core_pd = ad[i] + bd[i] + cd[i] + dd[i] + ed[i];								// Total core dynamic power
				core_pl = (a - ad[i]) + (b - bd[i]) + (c - cd[i]) + (d - dd[i]) + (e - ed[i]);	// Total core leakage power

//				cerr << "Reg_Sam_Iter= " << reg_sam_iter << endl;
				TArray[i][reg_sam_iter] = core_t;
				Pd[i][reg_sam_iter] = core_pd;
				Pl[i][reg_sam_iter] = core_pl;
				if (Performance[i][reg_sam_iter] == 0)	// If no instructions are fetched, ops/byte = 0;
					Ops_Byte[i][reg_sam_iter] = 0;
				else
				{
					if ((reads[i] + writes[i]) == 0 )
						Ops_Byte[i][reg_sam_iter] = (Performance[i][reg_sam_iter]*p_cnt->period*1e6);	// Avoiding inf
					else
						Ops_Byte[i][reg_sam_iter] = (Performance[i][reg_sam_iter]*p_cnt->period*1e6)/(reads[i]*64);	// Not using write backs because they don't truly reflect
																													// mem boundedness.
				}


				cerr << "Core" << i << "\tP_D\t" << core_pd
				                    << "\tP_L\t" << core_pl
				                    << "\tAVG_T\t" << core_t
				                    << "\tMAX_T\t" << *(std::max_element(CORE_T.begin(), CORE_T.end()))
				     << "\tL2CACHE" << i << "\tP_D\t" << fd[i] << "\tP_L\t" << fl << "\tT\t" << L2CACHE_T
				     << "\tVAULT" << i << "\tLAYER0\tP_D\t" << dram0_d[i] << "\tP_L\t" << dram_leak_power[0] << "\tAVG_T\t" << DRAM_T[0]
				                       << "\tLAYER1\tP_D\t" << dram1_d[i] << "\tP_L\t" << dram_leak_power[1] << "\tAVG_T\t" << DRAM_T[1]
				                       << "\tLAYER2\tP_D\t" << dram2_d[i] << "\tP_L\t" << dram_leak_power[2] << "\tAVG_T\t" << DRAM_T[2]
				                       << "\tLAYER3\tP_D\t" << dram3_d[i] << "\tP_L\t" << dram_leak_power[3] << "\tAVG_T\t" << DRAM_T[3]
				     << "\tMEM_READS\t" << reads[i] << "\tMEM_WRITES\t" << writes[i] << "\tOps_Byte\t" << Ops_Byte[i][reg_sam_iter]
				                                                                                       << endl << flush;

			}


			double time = manifold::kernel::Clock::Master().NowTime();

			cerr << "inst_power\t" << ip << "\tinst_leak\t" << il
			     <<"\tinst_power_CORE\t" << ipp << "\tinst_leak_CORE\t" << ilp
			     <<"\tinst_power_L2CACHE\t" << ips << "\tinst_leak_L2CACHE\t" << ils
			     <<"\tinst_power_DRAM\t" << inst_dram_p << "\tinst_leak_DRAM\t" << inst_dram_l << endl << flush;

			cerr << "total_energy\t" << total_energy << "\ttotal_leak_energy\t" << total_leakage
			     << "\ttotal_energy_CORE\t" << core_energy << "\ttotal_leak_energy_CORE\t" << core_leakage
			     << "\ttotal_energy_L2CACHE\t" << sram_energy << "\ttotal_leak_energy_L2CACHE\t" << sram_leakage
			     << "\ttotal_energy_DRAM\t" << dram_energy << "\ttotal_leak_energy_DRAM\t" << dram_leakage_energy << endl << flush;

			cerr << "average_power\t" << total_energy/time <<"\taverage_leakage\t" << total_leakage/time
			     << "\taverage_power_CORE\t" << core_energy/time << "\taverage_leak_CORE\t" << core_leakage/time
			     << "\taverage_power_L2CACHE\t"<< sram_energy/time << "\taverage_leak_L2CACHE\t" << sram_leakage/time
			     << "\taverage_power_DRAM\t" << dram_energy/time << "\taverage_leak_DRAM\t" << dram_leakage_energy/time << endl << flush;

			// Enable this part to print grid temperature
#if 0
			cerr << "CORE_DIE**************************************************" << endl;
			EI::grid_t<double> thermal_grid = ei->pull_data<EI::grid_t<double> >(p_cnt->time_tick,"package","16CORE","thermal_grid");
			for(int col_index = 0; col_index < 50; col_index++) {
			for(int row_index = 0; row_index < 50; row_index++)
			  cerr << thermal_grid.pull(row_index,col_index,0) << ",";
			  cerr << endl;
			 }

			cerr << "CACHE_DIE*************************************************" << endl;
			for(int col_index = 0; col_index < 50; col_index++) {
			for(int row_index = 0; row_index < 50; row_index++)
			  cerr << thermal_grid.pull(row_index,col_index,1) << ",";
			  cerr << endl;
			}

			cerr << "DRAM_DIE0*************************************************" << endl;
			for(int col_index = 0; col_index < 50; col_index++) {
			for(int row_index = 0; row_index < 50; row_index++)
			  cerr << thermal_grid.pull(row_index,col_index,2) << ",";
			  cerr << endl;
			}

			cerr << "DRAM_DIE1*************************************************" << endl;
			for(int col_index = 0; col_index < 50; col_index++) {
			for(int row_index = 0; row_index < 50; row_index++)
			  cerr << thermal_grid.pull(row_index,col_index,3) << ",";
			  cerr << endl;
			}

			cerr << "DRAM_DIE2*************************************************" << endl;
			for(int col_index = 0; col_index < 50; col_index++) {
			for(int row_index = 0; row_index < 50; row_index++)
			  cerr << thermal_grid.pull(row_index,col_index,4) << ",";
			  cerr << endl;
			}

			cerr << "DRAM_DIE3*************************************************" << endl;
			for(int col_index = 0; col_index < 50; col_index++) {
			for(int row_index = 0; row_index < 50; row_index++)
			  cerr << thermal_grid.pull(row_index,col_index,5) << ",";
			  cerr << endl;
			}
			cerr << "**************************************************" << endl;
#endif
			for(int i=0;i<NUM_CORES;i++)
			{
				reads[i] = 0; writes[i] = 0;
			}
			reg_sam_iter += 1;
		}
#endif
	}

#endif


	if(sam_cycle == SAMPLING_CYCLE)
	{
		char ModuleID[64];
		tick_t num_clock_cycles_this_period = 0;

		p_cnt->time_tick += p_cnt->period; // update time_tick
#ifdef EI_COMPUTE
		//***Nehalem:Frontend***
		//core:instruction_buffer
		sprintf(ModuleID,"core:instruction_buffer:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->inst_buffer * factor);

		//core:BTB
		sprintf(ModuleID,"core:BTB:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),(p_cnt->l1_btb + p_cnt->l2_btb) * factor);

		//core:chooser
		sprintf(ModuleID,"core:chooser:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->predictor_chooser * factor);

		//core:global_predictor
		sprintf(ModuleID,"core:global_predictor:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->global_predictor * factor);

		//core:L1_local_predictor
		sprintf(ModuleID,"core:L1_local_predictor:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->l1_local_predictor * factor);

		//core:L2_local_predictor
		sprintf(ModuleID,"core:L2_local_predictor:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->l2_local_predictor * factor);

		//core:ITLB
		sprintf(ModuleID,"core:ITLB:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->inst_tlb * factor);

		//core:IL1
		sprintf(ModuleID,"core:IL1:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->inst_cache * factor);

		//core:IL1:MissBuffer
		sprintf(ModuleID,"core:IL1:MissBuffer:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->inst_cache_miss_buffer * factor);

		//core:IL1:FillBuffer //FIXME
		//sprintf(ModuleID,"core:IL1:FillBuffer:%d",id);
		//p_cnt->IL1.linefill = p_cnt->IL1.linefill * 2;
		//ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->IL1.linefill * factor);

		//core:IL1:PrefetchBuffer
		sprintf(ModuleID,"core:IL1:PrefetchBuffer:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->inst_cache_prefetch_buffer * factor);

		//core:instruction_decoder
		sprintf(ModuleID,"core:instruction_decoder:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->inst_decoder * factor);

		//core:operand_decoder
		sprintf(ModuleID,"core:operand_decoder:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->operand_decoder * factor);

		//core:uop_sequencer
		sprintf(ModuleID,"core:uop_sequencer:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->uop_sequencer * factor);

		//core:uop_queue
		sprintf(ModuleID,"core:uop_queue:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->uop_queue * factor);

		//core:RAS
		sprintf(ModuleID,"core:RAS:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->ras * factor);

		//***Nehalem:Schedule***
		//core:RS
		sprintf(ModuleID,"core:RS:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->rs * factor);

		//core:ROB
		sprintf(ModuleID,"core:ROB:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->rob * factor);

		//core:issue_select
		sprintf(ModuleID,"core:issue_select:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->issue_select * factor);

		//core:GPREG
		sprintf(ModuleID,"core:GPREG:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->reg_int * factor);

		//core:SCFREG //FIXME
		//sprintf(ModuleID,"core:SCFREG:%d",id);
		//ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->registers.segment
		  //                 + p_cnt->registers.control);

		//core:FPREG
		sprintf(ModuleID,"core:FPREG:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->reg_fp * factor);

		//core:RAT
		sprintf(ModuleID,"core:RAT:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->rat * factor);

		//core:freelist
		sprintf(ModuleID,"core:freelist:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->freelist * factor);

		//core:dependency_check
		sprintf(ModuleID,"core:dependency_check:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->dependency_check * factor);

		//***Nehalem:Execute:INT***
		//core:ALU
		sprintf(ModuleID,"core:ALU:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->alu * factor);

		//core:ALU_bypass
		sprintf(ModuleID,"core:ALU_bypass:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->int_bypass * factor);

		//***Nehalem:Execute:FP***
		//core:FPU
		sprintf(ModuleID,"core:FPU:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->fpu * factor);

		//core:MUL
		sprintf(ModuleID,"core:MUL:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->mul * factor);

		//core:FPU_bypass
		sprintf(ModuleID,"core:FPU_bypass:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->fp_bypass * factor);

		//core:MUL_bypass //FIXME
		//sprintf(ModuleID,"core:MUL_bypass:%d",id);
		//p_cnt->mul_bypass = p_cnt->mul_bypass * 5;
		//ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->mul_bypass);

		//core:StoreQueue
		sprintf(ModuleID,"core:StoreQueue:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->stq * factor);

		//core:LoadQueue
		sprintf(ModuleID,"core:LoadQueue:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->ldq * factor);

		//core:LD_bypass //FIXME
		//sprintf(ModuleID,"core:LD_bypass:%d",id);
		//p_cnt->load_bypass = p_cnt->load_bypass * 5;
		//ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->load_bypass);

		//***Wiring & Latch
		//core:PC
		sprintf(ModuleID,"core:PC:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->pc * factor);

		//core:pipe:IB2ID
		sprintf(ModuleID,"core:pipe:IB2ID:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_ib2id * factor);

		//core:pipe:ID2uQ
		sprintf(ModuleID,"core:pipe:ID2uQ:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_id2uq * factor);

		//core:pipe:uQ2RR
		sprintf(ModuleID,"core:pipe:uQ2RR:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_uq2rat * factor);

		//core:pipe:RR2SCH
		sprintf(ModuleID,"core:pipe:RR2SCH:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_rat2rs * factor);

		//core:pipe:SCH2EXP
		sprintf(ModuleID,"core:pipe:SCH2EXP:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_rs2ex * factor);

		//core:pipe:EXP2ALU
		sprintf(ModuleID,"core:pipe:EXP2ALU:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_ex_int* factor);

		//core:pipe:EXP2FPU
		sprintf(ModuleID,"core:pipe:EXP2FPU:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->latch_ex_fp * factor);

		//core:pipe:ALU2ROB
		sprintf(ModuleID,"core:pipe:ALU2ROB:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),p_cnt->alu * factor);

		//core:pipe:FPU2ROB
		sprintf(ModuleID,"core:pipe:FPU2ROB:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),(p_cnt->fpu + p_cnt->mul)* factor);

		//core:pipe:ROB2CM
		sprintf(ModuleID,"core:pipe:ROB2CM:%d",id);
		ei->compute_power(p_cnt->time_tick,p_cnt->period,string(ModuleID),(p_cnt->latch_rob2rs + p_cnt->latch_rob2arf + p_cnt->latch_ldq2rs) * factor);
#endif
		total_mips += p_cnt->retire_inst.read/p_cnt->period/1e6;

		num_clock_cycles_this_period = clock->NowTicks() - num_cycles_old;
		Performance[id][reg_sam_iter] = p_cnt->retire_inst.read/p_cnt->period/1e6;
		IPCArray[id][reg_sam_iter] = ((float)p_cnt->retire_inst.read)/num_clock_cycles_this_period;
		if(p_cnt->fetch_inst.read == 0)
		{
			CPU_Load[id][reg_sam_iter] = 0.0;
		}
		else
		{
			CPU_Load[id][reg_sam_iter] = 1.0 - (double(p_cnt->nop_inst.read) / double(p_cnt->fetch_inst.read));
		}

		cerr << "CORE" << id << "\tcycle\t" << clock->NowTicks() << "\tIPC_inst\t" << IPCArray[id][reg_sam_iter]
				             << "\tMIPS\t" << Performance[id][reg_sam_iter] << "\ttotal_MIPS\t" << total_mips
						     << "\tmem_reads\t" << p_l2cache->memreads<< "\tmem_writes\t"<< p_l2cache->memwrites
							 << "\tfetched_inst\t" << p_cnt->fetch_inst.read << "\tnop_inst\t" << p_cnt->nop_inst.read
							 << "\tclock_freq\t" << clock->freq << "\tVdd\t" << V_Old
							 << "\tnum_cycles\t" << num_clock_cycles_this_period << "\tCPU_Load\t" << CPU_Load[id][reg_sam_iter] << endl << flush;

		for(int i = 0; i < NUM_CORES; i++)
		{
			reads[i] += p_l2cache->mem_reads[i];
			writes[i] += p_l2cache->mem_writes[i];
		}
		p_cnt->reset();
		p_l2cache->clear_my_counters();
		p_l2cache->clear_mem_counters();
		num_cycles_old = clock->NowTicks();
	}
}
