#include "kernel/component.h"
#include "kernel/manifold.h"
#include "dram_sim.h"
//#include "Transaction.h"

#include <iostream>

using namespace std;
using namespace manifold::kernel;

namespace manifold {
namespace dramsim {

int Dram_sim:: MEM_MSG_TYPE = -1;
int Dram_sim :: CREDIT_MSG_TYPE = -1;
bool Dram_sim :: Msg_type_set = false;


Dram_sim::Dram_sim (int nid, const Dram_sim_settings& dram_settings, Clock* clk, uint64_t nc) :
    m_nid(nid), dram_clk(clk), num_cores(nc)
{
    assert(Msg_type_set);
    assert(MEM_MSG_TYPE != CREDIT_MSG_TYPE);

    m_send_st_response = dram_settings.send_st_resp;
    downstream_credits = dram_settings.downstream_credits;
    MAX_DOWNSTREAM_CREDITS = downstream_credits;

    /* instantiate the DRAMSim module */
    mem = new MultiChannelMemorySystem(dram_settings.dev_filename, dram_settings.mem_sys_filename, ".", "res", dram_settings.size);

    mc_map = NULL;

    /* create and register DRAMSim callback functions */
    read_cb = new Callback<Dram_sim, void, unsigned, uint64_t, uint64_t>(this, &Dram_sim::read_complete);
    write_cb = new Callback<Dram_sim, void, unsigned, uint64_t, uint64_t>(this, &Dram_sim::write_complete);
    mem->RegisterCallbacks(read_cb, write_cb, NULL);
    mem->setCPUClockSpeed((uint64_t)dram_clk->freq);	// To do memory DFS, changing dram_sim wrapper frequency and the internal clock of dramsim2 is essential

    //register with clock
    Clock :: Register(*dram_clk, this, &Dram_sim::tick, (void(Dram_sim::*)(void)) 0 );

    DRAMOperationMode = true;
    DRAM_DFS_option = 1;

    //stats
    stats_n_reads = 0;
    stats_n_writes = 0;
    stats_n_reads_sent = 0;
    stats_totalMemLat = 0;

    // TODO: NUM_CORES needs to be passed here
    inst_stats_n_reads_per_source.resize(num_cores);

    cerr << "Created DRAMSim2 with cid: " << this->getComponentId() << " nodeid: " << this->get_nid() << " clock(Hz): " << dram_clk->freq << endl << flush;
/*
    uint64_t temp_freq = 800000000;
    if (this->get_nid() == 1) {
    	if(!mem->setDRAMTiming(temp_freq, 1)) {
			cerr << " MC unable to change to DFS option " << DRAM_DFS_option << " retaining previous." << endl;
		}
    	else {
    		cerr << " MC" << this->get_nid() << " freq changed to: " << temp_freq << " DRAM_DFS_option " << DRAM_DFS_option << endl;
    		mem->PrintDRAMTimings();
    	}

    }
*/
#ifdef DBG_DRAMSIM
    cout << "Created DRAM with id: " << m_nid << endl;
#endif
#ifdef DRAMSIM_UTEST
    completed_writes = 0;
#endif
}


void Dram_sim::read_complete(unsigned id, uint64_t address, uint64_t done_cycle)
{
    //cout << "@ " << m_clk->NowTicks() << " (local) " << Manifold::NowTicks() << " (default), read complete\n";
    map<uint64_t, list<Request> >::iterator it = m_pending_reqs.find(address);
    assert (it != m_pending_reqs.end());

    Request req = m_pending_reqs[address].front();
    m_pending_reqs[address].pop_front();
    if (it->second.size() == 0)
    m_pending_reqs.erase(it);

    assert(req.read);
    assert(req.addr == address);

    m_completed_reqs.push_back(req);
}


void Dram_sim::write_complete(unsigned id, uint64_t address, uint64_t done_cycle)
{
    map<uint64_t, list<Request> >::iterator it = m_pending_reqs.find(address);
    assert (it != m_pending_reqs.end());

    Request req = m_pending_reqs[address].front();
    m_pending_reqs[address].pop_front();
    if (it->second.size() == 0)
    m_pending_reqs.erase(it);

    assert(req.read == false);
    assert(req.addr == address);

    //move from pending buffer to completed buffer
    if (m_send_st_response) {
    m_completed_reqs.push_back(req);
    }

#ifdef DRAMSIM_UTEST
    completed_writes++;
#endif
}


void Dram_sim :: try_send_reply()
{
    if ( !m_completed_reqs.empty() && downstream_credits > 0) {
        //stats
    stats_n_reads_sent++;
    stats_totalMemLat += (m_clk->NowTicks() - m_completed_reqs.front().r_cycle);

    Request req = m_completed_reqs.front();
    m_completed_reqs.pop_front();

    //reuse the network packet object.
    manifold::uarch::NetworkPacket * pkt = (manifold::uarch::NetworkPacket*)(req.extra);
    pkt->type = MEM_MSG_TYPE;

//    *((uarch::Mem_msg*)(pkt->data)) = mem_msg;
//    pkt->data_size = sizeof(uarch::Mem_msg);

//    uarch::mcp_cache_namespace::Mem_msg mem_msg(req.gaddr, req.read);
//	mem_msg.type = 1;  // MEM RPLY
#ifdef DBG_DRAMSIM
    cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\treply LD\t\t\tsrc_id\t"
    		    << pkt->get_src() << "\tsrc_port\t" << pkt->get_src_port()
                << "\tdst_id\t" << pkt->get_dst() << "\tdst_port\t" << pkt->get_dst_port()
                << "\tladdr\t" << hex << req.addr << "\tgaddr\t" << req.gaddr << dec << endl;
#endif
    Send(PORT0, pkt);
    downstream_credits--;
#ifdef DBG_DRAMSIM
    cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\tdownstream_credits[" << downstream_credits+1 << "]->"
         << downstream_credits << endl;
#endif
    }
}


void Dram_sim :: send_credit()
{
    manifold::uarch::NetworkPacket *credit_pkt = new manifold::uarch::NetworkPacket();
    credit_pkt->type = CREDIT_MSG_TYPE;
    Send(PORT0, credit_pkt);
#ifdef DBG_DRAMSIM
    cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\tSending CREDIT PKT to xbar" << endl;
#endif
}


bool Dram_sim::limitExceeds()
{
    //TODO this is the request reply dependency
    // PendingRequest has to be limited. Cannot exceed indefinitely
    return (m_completed_reqs.size() > 8); // some low threshold
}


void Dram_sim::tick()
{
    //cout << "Dram sim tick(), t= " << m_clk->NowTicks() << endl;
    //start new transaction if there is any and the memory can accept
    if (!m_incoming_reqs.empty() && mem->willAcceptTransaction() && !limitExceeds() && DRAMOperationMode)
    {
		// if limit exceeds, stop sending credits. interface will stop eventually
		Request req = m_incoming_reqs.front();
		m_incoming_reqs.pop_front();
		m_incoming_reqs_size.push_back(m_incoming_reqs.size());

		mem->addTransaction(!req.read, req.addr);
#ifdef DBG_DRAMSIM
    cout << "@ " << dram_clk->NowTicks() << " MC " << m_nid << ": transaction of address " << hex << req.gaddr << dec << " is pushed to memory" << endl;
#endif
    	//move from input buffer to pending buffer
        m_pending_reqs[req.addr].push_back(req);
        send_credit();
    }

	if (!DRAMOperationMode)
	{
		if (mem->IsQueueEmpty())
		{
			cerr << "@ " << dram_clk->NowTicks() << " MC " << m_nid << " queue is empty." << endl;
			//FunctionToChangeDRAMTimings();
			if(!mem->setDRAMTiming((uint64_t)dram_clk->freq, DRAM_DFS_option))
			{
				cerr << "@ " << dram_clk->NowTicks() << " MC unable to change to DFS option " << DRAM_DFS_option << " retaining previous." << endl;
			}
			mem->PrintDRAMTimings();
			DRAMOperationMode = true;
		}
	}

//    if(dram_clk->NowTicks() % 100000 == 0)
//    {
//    	power_bw_data = mem->getIntervalPowerBWStatsMCMS();
//    	cerr << "@\t" << dram_clk->NowTicks();
//    	for(int i = 0; i < NUM_RANKS; i++)
//    	{
//    		cerr << "\tRank" << i << "\tAvg Pwr\t" << power_bw_data.AveragePower[i] << "W"
//    		     << "\tTotalBytesTxed\t" << power_bw_data.totalBytes << "\tAggregateBW\t" << power_bw_data.BandWidth << "GB/s" << endl;
//    	}
//    }
    mem->update();
    try_send_reply();

}

avgPowerBW* Dram_sim::getDRAMPowerBWStats(unsigned long cycles, unsigned DRAM_freq_option)
{
	double DRAM_freq_scaling = 1;
	switch (DRAM_freq_option)
	{
	case 1:
		// DRAM @ 1600 MT/s
		DRAM_freq_scaling = 1;
		break;
	case 2:
		// DRAM @ 1333 MT/s
		DRAM_freq_scaling = 0.83375;
		break;
	case 3:
		// DRAM @ 1066 MT/s
		DRAM_freq_scaling = 0.66625;
		break;
	case 4:
		// DRAM @ 800 MT/s
		DRAM_freq_scaling = 0.5;
		break;
	default:
		// DRAM @ 1600 MT/s
		DRAM_freq_scaling = 1;
		break;
	}
	power_bw_data = mem->getIntervalPowerBWStatsMCMS(cycles, DRAM_freq_scaling);
//	for(int i = 0; i < NUM_RANKS; i++)
//	{
//		cerr << "\tRank" << i << "\tAvg Pwr\t" << power_bw_data.AveragePower[i] << "W"
//			 << "\tTotalBytesTxed\t" << power_bw_data.totalBytes << "\tAggregateBW\t" << power_bw_data.BandWidth << "GB/s" << endl << flush;
//	}
	return &power_bw_data;
}

void Dram_sim::changeDRAMTiming(unsigned option)
{
	DRAMOperationMode = false;
	DRAM_DFS_option = option;
}

double Dram_sim::getDRAMFrequency()
{
	return mem->getDRAMFrequency();
}
void Dram_sim::PrintDRAMInstStats()
{
	uint64_t total_reads = std::accumulate(inst_stats_n_reads_per_source.begin(), inst_stats_n_reads_per_source.end(), 0);
	uint64_t total_writes = std::accumulate(inst_stats_n_writes_per_source.begin(), inst_stats_n_writes_per_source.end(), 0);

	if (total_reads > 0)
	{
		for (int i = 0; i < num_cores; i++)
		{
			cerr << "\tReads" << i << "\t" << inst_stats_n_reads_per_source[i];
		}
	}
	else
	{
		for (int i = 0; i < num_cores; i++)
		{
			cerr << "\tReads" << i << "\t0";
		}
	}
/*
	switch (m_nid)
	{
	case 0:
		if (total_reads > 0)
		{

//			cerr << "\tReads1\t" << (double)(inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2]) / total_reads
//			 << "\tReads2\t" << (double)inst_stats_n_reads_per_source[3] / total_reads
//			 << "\tReadsX\t" << (double)(inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[6] +
//								inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[9] +
//								inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[12] +
//								inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] + inst_stats_n_reads_per_source[15]) / total_reads;

			cerr << "\tReads0\t" << (double)(inst_stats_n_reads_per_source[0]) / total_reads
				 << "\tReads1\t" << (double)(inst_stats_n_reads_per_source[1]) / total_reads
				 << "\tReads2\t" << (double)(inst_stats_n_reads_per_source[2]) / total_reads
				 << "\tReads3\t" << (double)(inst_stats_n_reads_per_source[3]) / total_reads;
		}
		else
		{
			//cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
			cerr << "\tReads0\t0\tReads1\t0\tReads2\t0\tReads3\t0";
		}
		break;
	case 1:
		if (total_reads > 0)
		{

//			cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[3] +
//							   inst_stats_n_reads_per_source[4]) / total_reads
//			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[6]) / total_reads
//			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[8] +
//								inst_stats_n_reads_per_source[9] +  inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] +
//								inst_stats_n_reads_per_source[12] + inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] +
//								inst_stats_n_reads_per_source[15]) / total_reads;

		cerr << "\tReads0\t" << (double)(inst_stats_n_reads_per_source[0]) / total_reads
			 << "\tReads1\t" << (double)(inst_stats_n_reads_per_source[1]) / total_reads
			 << "\tReads2\t" << (double)(inst_stats_n_reads_per_source[2]) / total_reads
			 << "\tReads3\t" << (double)(inst_stats_n_reads_per_source[3]) / total_reads;
		}
		else
		{
			//cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
			cerr << "\tReads0\t0\tReads1\t0\tReads2\t0\tReads3\t0";
		}
		break;
	case 2:
		if (total_reads > 0)
		{

//		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[3] +
//							   inst_stats_n_reads_per_source[8]) / total_reads
//			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[9]) / total_reads
//			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[6] +
//								inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] +
//								inst_stats_n_reads_per_source[12] + inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] +
//								inst_stats_n_reads_per_source[15]) / total_reads;

		cerr << "\tReads0\t" << (double)(inst_stats_n_reads_per_source[0]) / total_reads
			 << "\tReads1\t" << (double)(inst_stats_n_reads_per_source[1]) / total_reads
			 << "\tReads2\t" << (double)(inst_stats_n_reads_per_source[2]) / total_reads
			 << "\tReads3\t" << (double)(inst_stats_n_reads_per_source[3]) / total_reads;
		}
		else
		{
			//cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
			cerr << "\tReads0\t0\tReads1\t0\tReads2\t0\tReads3\t0";
		}
		break;
	case 3:
		if (total_reads > 0)
		{

//		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[3] +
//							   inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[9]) / total_reads
//			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[8] +
//								 inst_stats_n_reads_per_source[12])/ total_reads
//			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[7]  + inst_stats_n_reads_per_source[10] +
//								inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] +
//								inst_stats_n_reads_per_source[15]) / total_reads;

		cerr << "\tReads0\t" << (double)(inst_stats_n_reads_per_source[0]) / total_reads
			 << "\tReads1\t" << (double)(inst_stats_n_reads_per_source[1]) / total_reads
			 << "\tReads2\t" << (double)(inst_stats_n_reads_per_source[2]) / total_reads
			 << "\tReads3\t" << (double)(inst_stats_n_reads_per_source[3]) / total_reads;
		}
		else
		{
			//cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
			cerr << "\tReads0\t0\tReads1\t0\tReads2\t0\tReads3\t0";
		}
		break;

	case 4:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] +
							   inst_stats_n_reads_per_source[6]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[7]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[8] +
								inst_stats_n_reads_per_source[9] + inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] +
								inst_stats_n_reads_per_source[12] + inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] +
								inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 5:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[7]) / total_reads
			 << "\tReads2\t" <<(double) inst_stats_n_reads_per_source[6] / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[9] +
								inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[12] +
								inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] + inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 6:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[6] +
							   inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[12]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[9] +
								 inst_stats_n_reads_per_source[13]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[8] +
								inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[14] +
								inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 7:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[7] +
							   inst_stats_n_reads_per_source[13]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[12]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[9] +
								inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[14] +
								inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 8:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[9] +
							   inst_stats_n_reads_per_source[10]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[11]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[4] +
								inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[7] +
								inst_stats_n_reads_per_source[12] + inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] +
								inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 9:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[9] +
							   inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[12]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[2] + inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[10] +
								 inst_stats_n_reads_per_source[14]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[4] +
								inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[13] +
								inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 10:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11]) / total_reads
			 << "\tReads2\t" <<(double) inst_stats_n_reads_per_source[9] / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] +
								inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[12] +
								inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] + inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 11:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[9] + inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11] +
							   inst_stats_n_reads_per_source[14]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[12]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] +
								inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[13] +
								inst_stats_n_reads_per_source[15]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 12:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[9] + inst_stats_n_reads_per_source[12] +
							   inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[11] +
								 inst_stats_n_reads_per_source[15]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] + inst_stats_n_reads_per_source[8] +
								inst_stats_n_reads_per_source[10]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 13:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[12] + inst_stats_n_reads_per_source[13] +
							   inst_stats_n_reads_per_source[15]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[14]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] +
								inst_stats_n_reads_per_source[8] + inst_stats_n_reads_per_source[9] + inst_stats_n_reads_per_source[10] +
								inst_stats_n_reads_per_source[11]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 14:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[11] + inst_stats_n_reads_per_source[12] + inst_stats_n_reads_per_source[14] +
							   inst_stats_n_reads_per_source[15]) / total_reads
			 << "\tReads2\t" <<(double) (inst_stats_n_reads_per_source[9] + inst_stats_n_reads_per_source[13]) / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] +
								inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[8] +
								inst_stats_n_reads_per_source[10]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;
	case 15:
		if (total_reads > 0)
		{
		cerr << "\tReads1\t" <<(double) (inst_stats_n_reads_per_source[13] + inst_stats_n_reads_per_source[14] + inst_stats_n_reads_per_source[15]) / total_reads
			 << "\tReads2\t" <<(double) inst_stats_n_reads_per_source[12] / total_reads
			 << "\tReadsX\t" <<(double) (inst_stats_n_reads_per_source[0] + inst_stats_n_reads_per_source[1] + inst_stats_n_reads_per_source[2] +
								inst_stats_n_reads_per_source[3] + inst_stats_n_reads_per_source[4] + inst_stats_n_reads_per_source[5] +
								inst_stats_n_reads_per_source[6] + inst_stats_n_reads_per_source[7] + inst_stats_n_reads_per_source[8] +
								inst_stats_n_reads_per_source[9] + inst_stats_n_reads_per_source[10] + inst_stats_n_reads_per_source[11]) / total_reads;
		}
		else
		{
			cerr << "\tReads1\t0\tReads2\t0\tReadsX\t0";
		}
		break;

	default:
		cerr << " Wrong node id. Terminating..." << endl;
		assert(0);
		break;
	}
*/
	ResetDRAMInstStats();
}

void Dram_sim::ResetDRAMInstStats()
{
	inst_stats_n_reads_per_source.clear();
	inst_stats_n_reads_per_source.resize(num_cores);
}

void Dram_sim :: set_mc_map(manifold::mcp_cache_namespace::PageBasedMap *m)
{
    this->mc_map = m;
    cerr << "DRAMSim2 node_id: " << this->m_nid << " PageBasedMap " << hex << m << dec << endl << flush;
}

void Dram_sim :: print_stats(ostream& out)
{
    out << "***** DRAMSim2 " << m_nid << " *****" << endl;
    out << "  Total Reads Received= " << stats_n_reads << endl;
    out << "  Total Writes Received= " << stats_n_writes << endl;
    out << "  Total Reads Sent Back= " << stats_n_reads_sent << endl;
    out << "  Avg Memory Latency= " << (double) ( (double)stats_totalMemLat / stats_n_reads_sent ) / dram_clk->freq << " s" << endl;
    out << "  Reads per source:" << endl;
    for(map<int, unsigned>::iterator it=stats_n_reads_per_source.begin(); it != stats_n_reads_per_source.end();
           ++it) {
        out << "    Core" << it->first << " : " << it->second << endl;
    }
    out << "  Writes per source:" << endl;
    for(map<int, unsigned>::iterator it=stats_n_writes_per_source.begin(); it != stats_n_writes_per_source.end();
           ++it) {
        out << "    " << it->first << " : " << it->second << endl;
    }
    if (m_incoming_reqs_size.size() != 0)
    {
    out << "  Average input buffer size " <<  ((double) std::accumulate(m_incoming_reqs_size.begin(), m_incoming_reqs_size.end(), 0) / (double) m_incoming_reqs_size.size())
        << "  Max input buffer size " << *(std::max_element(m_incoming_reqs_size.begin(), m_incoming_reqs_size.end()))
        << "  Min input buffer size " << *(std::min_element(m_incoming_reqs_size.begin(), m_incoming_reqs_size.end())) << endl;
    }
    else
    {
    out << "  Average input buffer size: 0 " << "  Max input buffer size: 0 " << "  Min input buffer size: 0 " << endl;
    }

    mem->printStats(true);
}



} // namespace dramsim
} //namespace manifold
