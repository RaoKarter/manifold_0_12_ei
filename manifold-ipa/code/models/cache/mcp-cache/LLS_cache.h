#ifndef MANIFOLD_MCP_CACHE_LLS_CACHE_H
#define MANIFOLD_MCP_CACHE_LLS_CACHE_H

#include "L2_cache.h"
#define CORE_COUNT 16


using namespace std;

namespace manifold {
namespace mcp_cache_namespace {

class MuxDemux;

class LLS_cache : public L2_cache {
public:
    friend class MuxDemux;

    enum {PORT_L1=0, PORT_LOCAL_L1};

    LLS_cache (int nid, const cache_settings&, const L2_cache_settings&);
    ~LLS_cache (void);

    void set_mux(MuxDemux* m)
    {
	assert(m_mux == 0);
        m_mux = m;
    }

    void handle_llp_incoming (int, Coh_msg*);

    void tick();

    void send_msg_to_l1(Coh_msg* msg);

    void print_stats(std::ostream&);

    manifold::uarch::NetworkPacket* pop_from_output_buffer();

    uint64_t mem_reads[CORE_COUNT];
    uint64_t mem_writes[CORE_COUNT];

    uint64_t memreads;
    uint64_t memwrites;

    void clear_mem_counters()
    {
    	for(int i = 0; i < CORE_COUNT; i++)
    	{
    		mem_reads[i] = 0;
    		mem_writes[i] = 0;
    	}
    }

    void clear_my_counters()
    {
    	memreads = 0;
    	memwrites = 0;
    }

private:
    MuxDemux* m_mux;
    std::list<Coh_msg*> m_llp_incoming;

    void add_to_output_buffer(manifold::uarch::NetworkPacket* pkt);

#ifdef MCP_CACHE_UTEST
public:
#else
protected:
#endif

    virtual void get_from_memory (Coh_msg*);
    virtual void dirty_to_memory (paddr_t);

    virtual void try_send();
    virtual void send_credit_downstream();

};


} //namespace mcp_cache
} //namespace manifold


#endif // MANIFOLD_MCP_CACHE_LLS_CACHE_H
