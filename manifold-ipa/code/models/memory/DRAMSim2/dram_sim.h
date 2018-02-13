#ifndef MANIDOLD_DRAM_SIM_H
#define MANIDOLD_DRAM_SIM_H


#include "kernel/component-decl.h"
#include "uarch/networkPacket.h"
//#include "uarch/memMsg.h"
#include "DRAMSim2-2.2.2/SystemConfiguration.h"
//#include "DRAMSim2-2.2.2/MemorySystem.h"
#include "DRAMSim2-2.2.2/MultiChannelMemorySystem.h"
#include "DRAMSim2-2.2.2/Transaction.h"
#include <list>
#include <map>
#include <iostream>
//#include <fstream>
#include <assert.h>
#include <vector>
#include <algorithm> // for max_element and min_element
#include <numeric> // for accumulate
#include "kernel/clock.h"
#include "models/cache/mcp-cache/MemoryControllerMap.h"


namespace manifold {
namespace dramsim {


struct Dram_sim_settings {
    Dram_sim_settings(const char* dev, const char* sys, int sz, bool st_resp, int credits) :
        dev_filename(dev), mem_sys_filename(sys), size(sz), send_st_resp(st_resp),
    downstream_credits(credits)
    {}
    const char* dev_filename;           // contains settings of the DRAM device
    const char* mem_sys_filename;       // Contains settings of the whole memory
    int size;                   // Memory size in MB
    //unsigned local_map;                   // Type of local address mapping
    bool send_st_resp;                  // whether responses are sent for stores
    int downstream_credits;
    //    manifold::uarch::DestMap* mc_map;
};



class Dram_sim : public manifold::kernel::Component {
public:
    enum {PORT0=0};

    Dram_sim(int nid, const Dram_sim_settings& dram_settings, manifold::kernel::Clock*, uint64_t nc);
    ~Dram_sim() {};

    int get_nid() { return m_nid; }

    void set_mc_map(manifold::mcp_cache_namespace::PageBasedMap *);

    template<typename T>
    void handle_request(int, manifold::uarch::NetworkPacket* req); // interface with NI

    void tick();        // registered with clock

    static void Set_msg_types(int mem, int credit) // Set some interface parameters
    {
        assert(Msg_type_set == false);
        MEM_MSG_TYPE = mem;
        CREDIT_MSG_TYPE = credit;
        Msg_type_set = true;
    }

    void print_stats(std::ostream&);
    void print_config(std::ostream&);

    avgPowerBW* getDRAMPowerBWStats(unsigned long, unsigned);
    void ResetDRAMInstStats();
    void PrintDRAMInstStats();
    void changeDRAMTiming(unsigned option);
    bool CheckMCQueueStatus() { return mem->IsQueueEmpty(); }

#ifdef DRAMSIM_UTEST
public:
#else
private:
#endif
    int MAX_DOWNSTREAM_CREDITS;
    struct Request {
        Request(uint64_t c, uint64_t a, uint64_t g, bool r, void* e) : r_cycle(c), addr(a), gaddr(g), read(r), extra(e) {}
        uint64_t r_cycle; //cycle when the request is first received
        uint64_t addr; //local address
        uint64_t gaddr; //global address
        bool read;   //true for read; false for write
        void* extra; //this is used to store the NetworkPacket for reuse when sending response.
                     //use void* so it can be used for other purposes.
    };

    static int MEM_MSG_TYPE;
    static int CREDIT_MSG_TYPE;
    static bool Msg_type_set;

    int m_nid;  // node id
    manifold::kernel::Clock* dram_clk;
    bool m_send_st_response;  // send response for stores
    int downstream_credits;     // NI credits`

    MultiChannelMemorySystem *mem;  // the original DRAMSim object

    manifold::mcp_cache_namespace::PageBasedMap *mc_map;

    std::list<Request> m_incoming_reqs;         // input buffer
    std::list<Request> m_completed_reqs;  // output buffer
    std::map<uint64_t, std::list<Request> > m_pending_reqs;    // buffer holding active requests,
                                                               // i.e., requests being processed

    /* create and register our callback functions */
    Callback_t *read_cb;
    Callback_t *write_cb;
//    Callback_t *power_cb;
    avgPowerBW power_bw_data;
    uint64_t num_cores;

    /* callbacks for read and write */
    void read_complete(unsigned id, uint64_t address, uint64_t done_cycle);
    void write_complete(unsigned id, uint64_t address, uint64_t done_cycle);

    bool limitExceeds();        // check if input has to be stopped because the output is full
    void try_send_reply();     // send reply if there's any and there's credit
    void send_credit();
    // This function sets a flag to true when operating in the normal mode i.e. fixed timings
    // Once the flag is false i.e. we need to change DRAM timings, it halts any new requests from
    // being sent to the MC until the MC queue is cleared. Once the past requests in the queue
    // are handled, DRAM timings are changed and the flag is set to true.
    bool DRAMOperationMode;	// true = fixed timings normal operation, false = waiting for MC queue to clear
    unsigned DRAM_DFS_option;	// 1 = 1600 MT/s, 2 = 1333 MT/s, 3 = 1066 MT/s, 4 = 800 MT/s

    //stat
    unsigned stats_n_reads;
    unsigned stats_n_writes;
    unsigned stats_n_reads_sent;
    uint64_t stats_totalMemLat;
    std::map<int, unsigned> stats_n_reads_per_source;
    std::map<int, unsigned> stats_n_writes_per_source;
    std::vector<int> inst_stats_n_reads_per_source;
    std::vector<int> inst_stats_n_writes_per_source; // Not used currently
    std::vector<unsigned int> m_incoming_reqs_size;

#ifdef DRAMSIM_UTEST
    unsigned completed_writes;
#endif
};


//! Event handler for incoming memory request.
template<typename T>
void Dram_sim :: handle_request(int in_port, manifold::uarch::NetworkPacket* pkt)
{
    if (pkt->type == CREDIT_MSG_TYPE) {
#ifdef DBG_DRAMSIM
        cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\tCurrent credits\t" << downstream_credits << endl;
#endif
    downstream_credits++;
    assert(downstream_credits <= MAX_DOWNSTREAM_CREDITS);
    delete pkt;
#ifdef DBG_DRAMSIM
    cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\trcvd CREDIT pkt @ port\t" << dec << in_port
            << "\tDRAMdownstream credits[" << this->get_nid() << "]\t" << downstream_credits-1 << "->" << downstream_credits << endl;
#endif
    return;
    }

//#ifndef HMCMEM
    assert (pkt->dst == m_nid);
//#endif

    T* req = (T*)(pkt->data);

    if (req->is_read())
    {
		stats_n_reads++;
		//stats_n_reads_per_source[pkt->get_src()]++;
		stats_n_reads_per_source[pkt->get_core_id()]++;
		inst_stats_n_reads_per_source[pkt->get_core_id()]++;
		req->set_mem_response(); //make it explicit the reply is a memory response. This is a temp solution!!!!!!!!!!!!!!!!!!
								 //The MC should send its own type.

		req->set_dst(pkt->get_src());
		req->set_dst_port(pkt->get_src_port());
		req->set_src(m_nid);
		req->set_src_port(0);
	#ifdef DBG_DRAMSIM
		cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\trcvd LD\t\t\t\tsrc_id\t" << pkt->get_src() << "\tsrc_port\t" << pkt->get_src_port()
					<< "\tdst_id\t" << pkt->get_dst() << "\tdst_port\t" << pkt->get_dst_port() << "\tladdr\t" << hex << mc_map->get_local_addr(req->get_addr())
					<< "\tgaddr\t" << req->get_addr() << dec << endl;
	#endif
    }
    else
    {
		stats_n_writes++;
		stats_n_writes_per_source[pkt->get_src()]++;
	#ifdef DBG_DRAMSIM
		cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\trcvd ST\t\t\t\tsrc_id\t" << pkt->get_src() << "\tsrc_port\t" << pkt->get_src_port()
					<< "\tdst_id\t" << pkt->get_dst() << "\tdst_port\t" << pkt->get_dst_port() << "\tladdr\t" << hex << mc_map->get_local_addr(req->get_addr())
					<< "\tgaddr\t" << req->get_addr() << dec << endl;
	#endif
    }

    //paddr_t newAddr = m_mc_map->ripAddress(req->addr);

//#ifndef HMCMEM
    pkt->set_dst(pkt->get_src());
    pkt->set_src(m_nid);
//#else
//    int temp = pkt->get_src();
//    pkt->set_src(pkt->get_dst());
//    pkt->set_dst(temp);
//#endif
    pkt->set_dst_port(pkt->get_src_port());
    pkt->set_src_port(0);
    pkt->type = 9;

    assert(mc_map);
#ifdef DBG_DRAMSIM
    cerr << dec << "@\t" << dram_clk->NowTicks() << "\tdram_clk\tVault\t" << this->get_nid() << "\tGoing to Process REQ \t\t\tsrc_id\t" << pkt->get_src() << "\tsrc_port\t" << pkt->get_src_port()
                << "\tdst_id\t" << pkt->get_dst() << "\tdst_port\t" << pkt->get_dst_port() << "\tladdr\t" << hex << mc_map->get_local_addr(req->get_addr()) << "\tgaddr\t" << req->get_addr() << dec << endl;
#endif
    //put the request in the input buffer
    m_incoming_reqs.push_back(Request(m_clk->NowTicks(), mc_map->get_local_addr(req->get_addr()), req->get_addr(), req->is_read(), pkt));
    m_incoming_reqs_size.push_back(m_incoming_reqs.size());
//    assert(m_incoming_reqs.size() <= MAX_DOWNSTREAM_CREDITS);
}




} // namespace dramsim
} // namespace manifold

#endif
