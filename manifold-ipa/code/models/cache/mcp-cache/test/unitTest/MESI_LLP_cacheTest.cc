//! This program tests MESI_LLP_cache. We use two Mock
//! components and connect the MESI_LLP_cache to the components.
//!
//!
//!                    ----------
//!                   | MockProc |
//!                    ----------
//!                         |
//!                  ----------------
//!                 | MESI_LLP_cache |
//!                  ----------------
//!                   |           |
//!                 --            ----------  <-- not Manifold link
//!                 |                       |
//!           -----------------         ---------
//!          | MockL2 plays L2 |       | MockMux |
//!           -----------------         ---------
//!                                         |
//!                                    ---------
//!                                   | MockNet |
//!                                    ---------

#include <TestFixture.h>
#include <TestAssert.h>
#include <TestSuite.h>
#include <Test.h>
#include <TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "MESI_LLP_cache.h"
#include "mux_demux.h"
#include "coherence/MESI_client.h"
#include "coh_mem_req.h"

#include "kernel/manifold-decl.h"
#include "kernel/manifold.h"
#include "kernel/component.h"
//#include "uarch/networkPacket.h"


using namespace std;
using namespace manifold::kernel;
using namespace manifold::uarch;
using namespace manifold::mcp_cache_namespace;

//####################################################################
// helper classes
//####################################################################
struct Proc_req {
    enum {LOAD, STORE};
    Proc_req(int rid, int nid, unsigned long ad, int t) :
        addr(ad), type(t)
	{}
    unsigned long get_addr() { return addr; }
    bool is_read() { return type == LOAD; }

    unsigned long addr;
    int type;
};


//! This class emulates a processor model.
class MockProc : public manifold::kernel::Component {
public:
    enum {OUT=0};
    enum {IN=0};
    void send_creq(Proc_req* creq)
    {
	Send(OUT, creq);
    }
    void handle_incoming(int, Proc_req* cresp)
    {
    //cout << "MockProc handle_incoming @ " << Manifold::NowTicks() << endl;
        m_cache_resps.push_back(*cresp);
        m_ticks.push_back(Manifold :: NowTicks());
    }
    vector<Proc_req>& get_cache_resps() { return m_cache_resps; }
    vector<Ticks_t>& get_ticks() { return m_ticks; }
private:
    vector<Proc_req> m_cache_resps;
    vector<Ticks_t> m_ticks;
};


//! This class emulates the local L2
class MockLower : public manifold::kernel::Component {
public:
    enum {OUT=0};
    enum {IN=0};

    void handle_incoming(int, Coh_msg* req)
    {
    //cout << "Lower called at " << Manifold :: NowTicks() << endl;
        m_mem_reqs.push_back(*req);
    }

    //with this function, the mock object can act as a manager
    void send_MESI(Coh_msg* msg)
    {
	Send(OUT, msg);
    }

    vector<Coh_msg>& get_mem_reqs() { return m_mem_reqs; }
    vector<NetworkPacket>& get_pkts() { return m_pkts; }

private:
    //Save the requests and timestamps for verification.
    vector<Coh_msg> m_mem_reqs;
    vector<NetworkPacket> m_pkts;
};

const int COH = 123;
const int CREDIT = 456;

//! This class subclasses MuxDemux.
class MockMux : public MuxDemux {
public:
    MockMux(MESI_LLP_cache* c) : m_llp_cache(c), MuxDemux(Clock::Master(), CREDIT)  {}

    void tick()
    {
        NetworkPacket* llp_pkt = m_llp_cache->pop_from_output_buffer(); 
	if(llp_pkt) {
	    m_pkts.push_back(*llp_pkt);
	    delete llp_pkt;
	}
    }

    void send_pkt(NetworkPacket* pkt)
    {
	m_llp_cache->handle_peer_and_manager_request(0, pkt);
    }

    void dummy(int, int) {} //dummy handler

    vector<NetworkPacket>& get_pkts() { return m_pkts; }
    vector<NetworkPacket>& get_credit_pkts() { return m_credit_pkts; }

private:
    MESI_LLP_cache* m_llp_cache;

    //Save the requests and timestamps for verification.
    vector<NetworkPacket> m_pkts;
    vector<NetworkPacket> m_credit_pkts;
};


//! This class emulates the network below the mux
class MockNet : public manifold::kernel::Component {
public:
    enum {PORT=0};

    void handle_incoming_packet(int, NetworkPacket* pkt)
    {
	if(pkt->type == CREDIT)
	    m_credit_pkts.push_back(*pkt);
	else {
	    assert(0);
	}
    }

    vector<Coh_msg>& get_mem_reqs() { return m_mem_reqs; }
    vector<NetworkPacket>& get_credit_pkts() { return m_credit_pkts; }

private:
    //Save the requests and timestamps for verification.
    vector<Coh_msg> m_mem_reqs;
    vector<NetworkPacket> m_pkts;
    vector<NetworkPacket> m_credit_pkts;
};



//! This mapping maps any address to the same id, which is initialized
//! in construction.
#if 0
class MyMcMap1 : public MemoryControllerMap {
public:
    MyMcMap1(int x) : m_mcid(x) {}
    int lookup(paddr_t addr) { return m_mcid; }
private:
    const int m_mcid;
};
#endif


const int NODE_ID = 5;
const int OTHER_NODE_ID = NODE_ID + 2;



//! This mapping maps (page % 4 == 0) to THE node and the rest to another node.
class MyPageMap : public DestMap {
public:
    int lookup(paddr_t addr)
    {
        if(((addr >> 12) & 0x3) == 0)
	    return NODE_ID;
	else
	    return OTHER_NODE_ID;
    }
};



//####################################################################
//! Class MESI_LLP_cacheTest is the test class for class MESI_LLP_cache. 
//####################################################################
class MESI_LLP_cacheTest : public CppUnit::TestFixture {
private:
    static const int HT_SIZE = 0x1 << 14; //2^14 = 16k;
    static const int HT_ASSOC = 4;
    static const int HT_BLOCK_SIZE = 32;
    static const int HT_HIT = 2;
    static const int HT_LOOKUP = 11;

    static const unsigned MSHR_SIZE = 8;

    //latencies
    static const Ticks_t PROC_CACHE = 1;
    static const Ticks_t CACHE_TO_L2 = 1;


    MyPageMap* m_l2map;
    MESI_LLP_cache* m_cachep;
    MockProc* m_procp;
    MockLower* m_l2p;
    MockMux* m_muxp;
    MockNet* m_netp;

public:
    static Clock MasterClock;  //clock has to be global or static.
    enum { MASTER_CLOCK_HZ = 10 };

    //! Initialization function. Inherited from the CPPUnit framework.
    void setUp()
    {
	cache_settings parameters;
	parameters.name = "testCache";
	//parameters.type = CACHE_DATA;
	parameters.size = HT_SIZE;
	parameters.assoc = HT_ASSOC;
	parameters.block_size = HT_BLOCK_SIZE;
	parameters.hit_time = HT_HIT;
	parameters.lookup_time = HT_LOOKUP;
	parameters.replacement_policy = RP_LRU;


        //create a MockProc, an LLP, and a mock MUX
	CompId_t procId = Component :: Create<MockProc>(0);
	m_procp = Component::GetComponent<MockProc>(procId);

	m_l2map = new MyPageMap(); //all addresses map to mc 0

	L1_cache_settings settings;
	settings.l2_map = m_l2map;
	settings.mshr_sz = MSHR_SIZE;
	settings.downstream_credits = 30;

	L1_cache :: Set_msg_types(COH, CREDIT);

	CompId_t cacheId = Component :: Create<MESI_LLP_cache>(0, NODE_ID, parameters, settings);
	m_cachep = Component::GetComponent<MESI_LLP_cache>(cacheId);
	CompId_t l2Id = Component :: Create<MockLower>(0);
	CompId_t muxId = Component :: Create<MockMux>(0, m_cachep);
	CompId_t netId = Component :: Create<MockNet>(0);

	m_l2p = Component::GetComponent<MockLower>(l2Id);
	m_muxp = Component::GetComponent<MockMux>(muxId);
	m_netp = Component::GetComponent<MockNet>(netId);

	m_cachep->set_mux(m_muxp);


        //connect the components
	//proc to L1
	Manifold::Connect(procId, MockProc::OUT, &MockProc::handle_incoming,
	                  cacheId, LLP_cache::PORT_PROC, &L1_cache::handle_processor_request<Proc_req>,
			  MasterClock, MasterClock, PROC_CACHE, PROC_CACHE);
	//L1 to L2
	Manifold::Connect(cacheId, LLP_cache::PORT_LOCAL_L2, &LLP_cache::handle_local_LLS_request,
	                  l2Id, MockLower::IN, &MockLower::handle_incoming,
			  MasterClock, MasterClock, CACHE_TO_L2, CACHE_TO_L2);

	//mux and mockNet
	Manifold::Connect(muxId, MuxDemux::PORT_NET, &MockMux::dummy,
	                  netId, MockNet::PORT, &MockNet::handle_incoming_packet,
			  MasterClock, MasterClock, 1, 1);

	Clock::Register(MasterClock, (LLP_cache*)m_cachep, &LLP_cache::tick, (void(LLP_cache::*)(void))0);
	Clock::Register(MasterClock, m_muxp, &MockMux::tick, (void(MockMux::*)(void))0);

    }






    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): load miss.
    //!
    //! Empty cache; load with a random address; since this is a miss, client
    //! sends MESI_CM_I_TO_S. Client enters IE state. There is an entry in the
    //! MSHR, the hash table, and the stall buffer.
    void test_handle_processor_request_load_l1_I_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    mreq = l2_mreqs[0];
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    NetworkPacket& pkt = mux_pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}


	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	//verify state of the MESI_LLP_cache
	//There should be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT(0 == m_cachep->stalled_client_req_buffer.size());

	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }



    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): load hit
    //!
    //! Empty cache; manually put a tag in the hash table and set the state of
    //! the corresponding client to E, S, or M. Send a request with the same
    //! address; it should hit, and a response is sent back. Client state is unchanged.
    //! There is no entry in the MSHR or the stall buffer.
    void test_handle_processor_request_load_l1_ESM_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	//manually put the ADDR in the hash table, and set the client state to
	//anything but I.
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	MESI_client_state_t valid_states[3] = {MESI_C_E, MESI_C_S, MESI_C_M};
	client->state = valid_states[random() % 3];
	MESI_client_state_t old_state = client->state;

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets NO Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(0, (int)l2_mreqs.size());
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(0, (int)mux_pkts.size());
	}



	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_LLP_cache
	//There should not be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should not be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	//verify state of the client unchanged.
	CPPUNIT_ASSERT_EQUAL(old_state, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): store miss.
    //!
    //! Empty cache; store with a random address; since this is a miss, client
    //! sends a MESI_CM_I_TO_E message. Client enters IE state.
    //! There should be an entry in the MSHR, the hash table, and the stall buffer.
    void test_handle_processor_request_store_l1_I_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    mreq = l2_mreqs[0];
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    NetworkPacket& pkt = mux_pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}


	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT(0 == m_cachep->stalled_client_req_buffer.size());

	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): store hit in M state
    //!
    //! Empty cache; manually put a tag in the hash table and set the state of
    //! the corresponding client to M. Send a request with the same
    //! address; it should hit, and a response is sent back. Client stays in M state.
    //! There is no entry in the MSHR or the stall buffer.
    void test_handle_processor_request_store_l1_M_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//manually put the ADDR in the hash table, and set the client state to
	//anything but I.
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_M;
	MESI_client_state_t old_state = client->state;

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets NO Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(0, (int)l2_mreqs.size());
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(0, (int)mux_pkts.size());
	}

	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	CPPUNIT_ASSERT_EQUAL(1, (int)cresps.size());
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should not be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	//verify state of the client unchanged.
	CPPUNIT_ASSERT_EQUAL(old_state, client->state);
    }



    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): store hit in E state
    //!
    //! Empty cache; manually put a tag in the hash table and set the state of
    //! the corresponding client to E. Send a request with the same
    //! address; it should hit, and a response is sent back. Client state changes to M.
    //! There is no entry in the MSHR or the stall buffer.
    void test_handle_processor_request_store_l1_E_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//manually put the ADDR in the hash table, and set the client state to
	//anything but I.
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_E;
	MESI_client_state_t old_state = client->state;

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets NO Coh_mem_req.
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(0, (int)l2_mreqs.size());
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(0, (int)mux_pkts.size());
	}


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	CPPUNIT_ASSERT_EQUAL(1, (int)cresps.size());
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should not be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	//verify state of the client changed to M.
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);
    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): store hit in S state
    //!
    //! Empty cache; manually put a tag in the hash table and set the state of
    //! the corresponding client to S. Send a request with the same
    //! address; it should hit; a MESI_CM_I_to_E request is sent to manager;
    //! client enters SE.
    //! There should be an entry in the MSHR, the hash table, and the stall buffer.
    void test_handle_processor_request_store_l1_S_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//manually put the ADDR in the hash table, and set the client state to S.
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_S;
	MESI_client_state_t old_state = client->state;

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets 1 Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    mreq = l2_mreqs[0];
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    NetworkPacket& pkt = mux_pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);


	//verify MockProc gets no response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	CPPUNIT_ASSERT_EQUAL(0, (int)cresps.size());

	//There should be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT(0 == m_cachep->stalled_client_req_buffer.size());

	//verify state of the client changes to SE
	CPPUNIT_ASSERT_EQUAL(MESI_C_SE, client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }
























//########
//response
//########

    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): load miss; manager in E; owner in E
    //!
    //! Empty cache; load with a random address; the following sequence happens:
    //! 1. Client --> manager, MESI_CM_I_to_S; client enters IE state.
    //! 2. Manager(in E) --> owner, MC_FWD_S.
    //! 3. Owner --> client, CC_S_DATA.
    //! 3. Client --> manager, CM_UNBLOCK_S; client enters S state.
    //! 4. L1 cache now sends a response back to processor.
    //!
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as the both forwarding client and manager.
    void test_handle_peer_and_manager_request_load_l1_I_l2_E_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg req;
	req.type = Coh_msg::COH_RPLY;
	req.addr = ADDR;
	req.msg = MESI_CC_S_DATA;

	NetworkPacket* ownerPkt = new NetworkPacket;
	ownerPkt->type = m_cachep->COH_MSG;
	ownerPkt->dst = NODE_ID;
	*((Coh_msg*)(ownerPkt->data)) = req;
	ownerPkt->data_size = sizeof(Coh_msg);
	Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, ownerPkt);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_S, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_S, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): load miss; manager in E; owner in M
    //!
    //! Empty cache; load with a random address; the following sequence happens:
    //! 1. Client --> manager, CM_I_to_S; client enters IE state.
    //! 2. Manager(in E) --> owner, MC_FWD_S.
    //! 3. Owner --> client, CC_S_DATA.
    //! 4. Client --> manager, MESI_CM_UNBLOCK_S; client enters S state.
    //! 4. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as the both forwarding client and manager.
    void test_handle_peer_and_manager_request_load_l1_I_l2_E_owner_M_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg req;
	req.type = Coh_msg::COH_RPLY;
	req.addr = ADDR;
	req.msg = MESI_CC_S_DATA;

	NetworkPacket* ownerPkt = new NetworkPacket;
	ownerPkt->type = m_cachep->COH_MSG;
	ownerPkt->dst = NODE_ID;
	*((Coh_msg*)(ownerPkt->data)) = req;
	ownerPkt->data_size = sizeof(Coh_msg);
	Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, ownerPkt);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_S, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_S, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): load miss; manager in S.
    //!
    //! Empty cache; load with a random address; the following sequence happens:
    //! 1. Client --> manager, CM_I_to_S; client enters IE state.
    //! 2. Manager(in S) --> client, MC_GRANT_S_DATA.
    //! 3. Client --> manager, CM_UNBLOCK_S ; client enters S state.
    //! 4. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as manager.
    void test_handle_peer_and_manager_request_load_l1_I_l2_S_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg req;
	req.type = Coh_msg::COH_RPLY;
	req.addr = ADDR;
	req.msg = MESI_MC_GRANT_S_DATA;

	NetworkPacket* mgrPkt = new NetworkPacket;
	mgrPkt->type = m_cachep->COH_MSG;
	mgrPkt->dst = NODE_ID;
	*((Coh_msg*)(mgrPkt->data)) = req;
	mgrPkt->data_size = sizeof(Coh_msg);
	Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}
	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_S, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_S, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): load miss; manager in I.
    //!
    //! Empty cache; load with a random address; the following sequence happens:
    //! 1. Client --> manager, CM_I_to_S; client enters IE state.
    //! 2. Manager --> client,  MC_GRANT_E_DATA; manager enters IE state.
    //! 3. Client --> manager, CM_UNBLOCK_E; client enters E state.
    //! 4. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as the manager.
    void test_handle_peer_and_manager_request_load_l1_I_l2_I_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_E, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): store miss; manager in E; owner in E.
    //!
    //! Empty cache; store with a random address; the following sequence happens:
    //! 1. Client --> manager, CM_I_to_E; client enters IE state.
    //! 2. Manager(in E) --> owner,  MC_FWD_E.
    //! 3. Owner --> client, CC_E_DATA.
    //! 4. Client --> manager, CM_UNBLOCK_E to manager; client enters E state, then to M state.
    //! 5. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as both owner and manager.
    void test_handle_peer_and_manager_request_store_l1_I_l2_E_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg req;
	req.type = Coh_msg::COH_RPLY;
	req.addr = ADDR;
	req.msg = MESI_CC_E_DATA;

	NetworkPacket* ownerPkt = new NetworkPacket;
	ownerPkt->type = m_cachep->COH_MSG;
	ownerPkt->dst = NODE_ID;
	*((Coh_msg*)(ownerPkt->data)) = req;
	ownerPkt->data_size = sizeof(Coh_msg);
	Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, ownerPkt);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);
    }



    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): store miss; manager in E; owner in M.
    //!
    //! Empty cache; store with a random address; the following sequence happens:
    //! 1. Client sends MESI_CM_I_to_E to manager; client enters IE state.
    //! 2. Manager in E state, so it sends MESI_MC_FWD_E to owner, which in turn sends
    //! MESI_CC_M_DATA to client.
    //! 3. Client sends MESI_CM_UNBLOCK_E to manager; client enters M state.
    //! 4. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as both owner and manager.
    void test_handle_peer_and_manager_request_store_l1_I_l2_E_owner_M_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg req;
	req.type = Coh_msg::COH_RPLY;
	req.addr = ADDR;
	req.msg = MESI_CC_M_DATA;

	NetworkPacket* ownerPkt = new NetworkPacket;
	ownerPkt->type = m_cachep->COH_MSG;
	ownerPkt->dst = NODE_ID;
	*((Coh_msg*)(ownerPkt->data)) = req;
	ownerPkt->data_size = sizeof(Coh_msg);
	Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, ownerPkt);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);
    }



    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): store miss; manager in S
    //!
    //! Empty cache; store with a random address; the following sequence happens:
    //! 1. Client sends MESI_CM_I_to_E to manager; client enters IE state.
    //! 2. Manager in S state, so it invalidates all shares, then sends
    //! MESI_MC_GRANT_E_DATA to client.
    //! 3. Client sends MESI_CM_UNBLOCK_E to manager; client enters E state, then M state.
    //! 4. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as manager.
    void test_handle_peer_and_manager_request_store_l1_I_l2_S_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);
	//CPPUNIT_ASSERT_EQUAL(ST_COMPLETE, cresp.msg);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);
    }



    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): store miss; manager in I.
    //!
    //! Empty cache; store with a random address; the following sequence happens:
    //! 1. Client sends MESI_CM_I_to_E to manager; client enters IE state.
    //! 2. Manager in I state, so it sends MESI_MC_GRANT_E_DATA to client; manager enters IE state.
    //! 3. Client sends MESI_CM_UNBLOCK_E to manager; client enters E state, then to M state.
    //! 4. L1 cache now sends a response back to processor.
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //! We use the mock object to act as the manager.
    void test_handle_peer_and_manager_request_store_l1_I_l2_I_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);
	//CPPUNIT_ASSERT_EQUAL(ST_COMPLETE, cresp.msg);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);
    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): store hit in S state
    //!
    //! Empty cache; manually put a tag in the hash table and set the state of
    //! the corresponding client to S. Send a request with the same
    //! address; it should hit; a MESI_CM_I_to_E request is sent to manager;
    //! client enters SE.
    //! When manager responds with MC_GRANT_E_DATA, client enters E state.
    //!
    //! 1. Client --> manager, CM_I_to_E; client enters SE
    //! 2. Manager --> client, MC_GRANT_E_DATA.
    //!
    //! At the end there should be an entry in the hash table, but not in the MSHR
    //! or the stall buffer.
    //!
    void test_handle_peer_and_manager_request_store_hit_l1_S_l2_S_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//manually put the ADDR in the hash table, and set the client state to S.
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_S;
	MESI_client_state_t old_state = client->state;

	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);

	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a manager response
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;


	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets CM_I_to_E and CM_UNBLOCK_E.
	Coh_msg mreq0, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size());
	    mreq0 = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt0 = pkts[0];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt0.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mreq0 = *((Coh_msg*)(pkt0.data));
	    mreq1 = *((Coh_msg*)(pkt1.data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq0.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);



	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(ADDR, cresp.addr);

	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT(0 == m_cachep->stalled_client_req_buffer.size());

	//verify state of the client changes to M
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);
	CPPUNIT_ASSERT(0 == m_cachep->mcp_stalled_req[client->getClientID()]);
    }






















//#####################
//Owner/sharer behavior
//#####################

    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): receive MESI_MC_FWD_E; owner in E
    //!
    //! Test a owner's behavior when it's in E state and receives MESI_MC_FWD_E: it sends
    //! MESI_CC_E_DATA and invalidates itself.
    //! 1. put ADDR in hash table and client in E state.
    //! 2. C<---M, MESI_MC_FWD_E;
    //! 4. C--->forwardee, MESI_CC_E_DATA; client invalidated.
    void test_handle_peer_and_manager_request_owner_E_recv_FWD_E_0()
    {
	const unsigned long ADDR = random();

	//put ADDR in hash table.
	m_cachep->my_table->reserve_block_for(ADDR);

	//put the client in E state
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;

        Coh_msg* req2 = new Coh_msg();
	req2->type = Coh_msg::COH_REQ;
	req2->addr = ADDR;
	req2->msg = MESI_MC_FWD_E;
	int FWD_ID;
	while((FWD_ID = random() % 1024) == NODE_ID); //the forward id is anything but NODE_ID.
	req2->forward_id = FWD_ID;

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR)); //invalidated!
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	CPPUNIT_ASSERT_EQUAL(MESI_C_I, client->state);

        //verify the downstream component gets Coh_mem_req.
	vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	CPPUNIT_ASSERT_EQUAL(1, (int)mux_pkts.size());
	NetworkPacket& pkt = mux_pkts[0];

	CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	CPPUNIT_ASSERT_EQUAL(FWD_ID, pkt.dst);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.dst_port);
	CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	//##### purpose of test case: verify this
	Coh_msg mreq0 = *((Coh_msg*)(pkt.data));
	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CC_E_DATA, mreq0.msg);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL(FWD_ID, mreq0.dst_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq0.dst_port);
    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): receive MESI_MC_FWD_E; owner in M
    //!
    //! Test a owner's behavior when it's in M state and receives MESI_MC_FWD_E: it sends
    //! MESI_CC_M_DATA and invalidates itself.
    //! 1. put ADDR in hash table and client in M state.
    //! 2. C<---M, MESI_MC_FWD_E;
    //! 3. C--->forwardee, MESI_CC_M_DATA; client invalidated.
    //!
    void test_handle_peer_and_manager_request_owner_M_recv_FWD_E_0()
    {
	const unsigned long ADDR = random();

	//put ADDR in hash table.
	m_cachep->my_table->reserve_block_for(ADDR);

	//put the client in M state
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;

        Coh_msg* req2 = new Coh_msg();
	req2->type = Coh_msg::COH_REQ;
	req2->addr = ADDR;
	req2->msg = MESI_MC_FWD_E;
	int FWD_ID;
	while((FWD_ID = random() % 1024) == NODE_ID); //the forward id is anything but NODE_ID.
	req2->forward_id = FWD_ID;

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR)); //invalidated!
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	CPPUNIT_ASSERT_EQUAL(MESI_C_I, client->state);


        //verify the downstream component gets Coh_mem_req.
	vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	CPPUNIT_ASSERT_EQUAL(1, (int)mux_pkts.size());
	NetworkPacket& pkt = mux_pkts[0];

	CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	CPPUNIT_ASSERT_EQUAL(FWD_ID, pkt.dst);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.dst_port);
	CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	//##### purpose of test case: verify this
	Coh_msg mreq0 = *((Coh_msg*)(pkt.data));
	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CC_M_DATA, mreq0.msg);

	CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	CPPUNIT_ASSERT_EQUAL(FWD_ID, mreq0.dst_id);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq0.dst_port);
    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): receive MESI_MC_FWD_S; owner in E
    //!
    //! Test a owner's behavior when it's in E state and receives MESI_MC_FWD_S: it sends
    //! MESI_CC_S_DATA and enters S state.
    //! 1. put ADDR in hash table and client in E state.
    //! 2. C<---M, MESI_MC_FWD_S;
    //! 3. C--->forwardee, MESI_CC_S_DATA
    //! 4. C--->Manager, MESI_CM_CLEAN; client enters S.
    void test_handle_peer_and_manager_request_owner_E_recv_FWD_S_0()
    {
	const unsigned long ADDR = random();

	//put ADDR in hash table.
	m_cachep->my_table->reserve_block_for(ADDR);

	//put the client in E state
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;

        Coh_msg* req2 = new Coh_msg();
	req2->type = Coh_msg::COH_REQ;
	req2->addr = ADDR;
	req2->msg = MESI_MC_FWD_S;
	int FWD_ID;
	while((FWD_ID = random() % 1024) == NODE_ID); //the forward id is anything but NODE_ID.
	req2->forward_id = FWD_ID;

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	CPPUNIT_ASSERT_EQUAL(MESI_C_S, client->state);

        //verify the downstream component gets Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	NetworkPacket& pkt0 = mux_pkts[0];

	CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt0.src_port);
	CPPUNIT_ASSERT_EQUAL(FWD_ID, pkt0.dst);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt0.dst_port);
	CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(1, (int)mux_pkts.size());
	    CPPUNIT_ASSERT_EQUAL(1, (int)l2_mreqs.size());

	    Coh_msg& mreq0 = *((Coh_msg*)(pkt0.data));
	    Coh_msg& mreq1 = l2_mreqs[0];

	    //##### purpose of test case: verify this
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_S_DATA, mreq0.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	    CPPUNIT_ASSERT_EQUAL(FWD_ID, mreq0.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq0.dst_port);

	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_CLEAN, mreq1.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), mreq1.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);
	}
	else {
	    CPPUNIT_ASSERT_EQUAL(2, (int)mux_pkts.size()); // CC_S_DATA, CM_CLEAN

	    NetworkPacket& pkt1 = mux_pkts[1];

	    Coh_msg& mreq0 = *((Coh_msg*)(pkt0.data));
	    Coh_msg& mreq1 = *((Coh_msg*)(pkt1.data));


	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    //##### purpose of test case: verify this
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_S_DATA, mreq0.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	    CPPUNIT_ASSERT_EQUAL(FWD_ID, mreq0.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq0.dst_port);

	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_CLEAN, mreq1.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), mreq1.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);
	}


    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): receive MESI_MC_FWD_S; owner in M
    //!
    //! Test a owner's behavior when it's in M state and receives MESI_MC_FWD_S: it sends
    //! MESI_CC_S_DATA and MESI_CM_WRITEBACK, and enters S state.
    //! 1. put ADDR in hash table and client in M state.
    //! 2. C<---M, MESI_MC_FWD_S;
    //! 3. C--->forwardee, MESI_CC_S_DATA
    //! 4. C--->M, MESI_CM_WRITEBACK
    //!
    void test_handle_peer_and_manager_request_owner_M_recv_FWD_S_0()
    {
	const unsigned long ADDR = random();

	//put ADDR in hash table.
	m_cachep->my_table->reserve_block_for(ADDR);

	//put the client in M state
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;

        Coh_msg* req2 = new Coh_msg();
	req2->type = Coh_msg::COH_REQ;
	req2->addr = ADDR;
	req2->msg = MESI_MC_FWD_S;
	int FWD_ID;
	while((FWD_ID = random() % 1024) == NODE_ID); //the forward id is anything but NODE_ID.
	req2->forward_id = FWD_ID;

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());


	//verify state of the client
	CPPUNIT_ASSERT_EQUAL(MESI_C_S, client->state);

        //verify the downstream component gets Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	NetworkPacket& pkt0 = mux_pkts[0];

	CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt0.type);
	CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt0.src);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt0.src_port);
	CPPUNIT_ASSERT_EQUAL(FWD_ID, pkt0.dst);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt0.dst_port);
	CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt0.data_size);

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(1, (int)mux_pkts.size());
	    CPPUNIT_ASSERT_EQUAL(1, (int)l2_mreqs.size());

	    Coh_msg& mreq0 = *((Coh_msg*)(pkt0.data));
	    Coh_msg& mreq1 = l2_mreqs[0];

	    //##### purpose of test case: verify this
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_S_DATA, mreq0.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	    CPPUNIT_ASSERT_EQUAL(FWD_ID, mreq0.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq0.dst_port);

	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_WRITEBACK, mreq1.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), mreq1.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);
	}
	else {
	    CPPUNIT_ASSERT_EQUAL(2, (int)mux_pkts.size()); // CC_S_DATA, CM_WRITEBACK

	    NetworkPacket& pkt1 = mux_pkts[1];

	    Coh_msg& mreq0 = *((Coh_msg*)(pkt0.data));
	    Coh_msg& mreq1 = *((Coh_msg*)(pkt1.data));


	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    //##### purpose of test case: verify this
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq0.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_S_DATA, mreq0.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq0.src_id);
	    CPPUNIT_ASSERT_EQUAL(FWD_ID, mreq0.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq0.dst_port);

	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_WRITEBACK, mreq1.msg);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, mreq1.src_id);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), mreq1.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);
	}
    }





















//########
//eviction
//########

    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): load miss; evicted in E;
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the evicted entry is in E, it sends MESI_CM_E_to_I to manager.
    void test_handle_processor_request_load_eviction_l1_E_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in E state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size());
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);


	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	//for the victim
	//There should NOT be an entry in the MSHR for the victim
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(VICTIM_ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(VICTIM_ADDR));

	//verify state of the victim's client
	CPPUNIT_ASSERT_EQUAL(MESI_C_EI, victim_client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[victim_client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[victim_client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): load miss; evicted in S;
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the evicted entry is in S, it transits to I state; no messages to the manager.
    void test_handle_processor_request_load_eviction_l1_S_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in S state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_S;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size()); //after the eviction; MESI_CM_I_to_S is sent.
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT(0 == m_cachep->stalled_client_req_buffer.size());

	//for the victim
	//There should NOT be an entry in the MSHR for the victim
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(VICTIM_ADDR));
	//There should NOT be an entry in the hash table - invalidated
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//verify state of the client;
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, victim_client->state); //victim_client becomes the new request's client
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[victim_client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[victim_client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): load miss; evicted in M;
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the evicted entry is in M, it sends MESI_CM_M_to_I to manager.
    void test_handle_processor_request_load_eviction_l1_M_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in M state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	Manifold::StopAt(scheduledAt + PROC_CACHE + HT_LOOKUP + 10);
	Manifold::Run();

        //verify the downstream component gets a Coh_msg.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size());
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	//for the victim
	//There should NOT be an entry in the MSHR for the victim
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(VICTIM_ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(VICTIM_ADDR));

	//verify state of the victim's client
	CPPUNIT_ASSERT_EQUAL(MESI_C_MI, victim_client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[victim_client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[victim_client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request() and handle_peer_and_manager_request():
    //! load miss; evicted in E;
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! 1. victim--->M, MESI_CM_E_to_I.
    //! 2. victim<---M, MESI_MC_GRANT_I.
    //! 3. victim--->M, MESI_CM_UNBLOCK_I.
    //!
    //! Now the new request is woken up.
    //! 1. C--->M, MESI_CM_I_to_S.
    void test_handle_peer_and_manager_request_load_eviction_l1_E_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();

	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;

	}

	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in E state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + 50;

        Coh_msg* req2 = new Coh_msg();
	req2->type = Coh_msg::COH_RPLY;
	req2->addr = VICTIM_ADDR;
	req2->msg = MESI_MC_GRANT_I;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + 50;


	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq, mreq1, mreq2;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(3, (int)mreqs.size());
	    mreq = mreqs[0];
	    mreq1 = mreqs[1];
	    mreq2 = mreqs[2];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(3, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    mreq1 = *((Coh_msg*)(pkt1.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    NetworkPacket& pkt2 = pkts[2];
	    mreq2 = *((Coh_msg*)(pkt2.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt2.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt2.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt2.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt2.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt2.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt2.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq2.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq2.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq2.dst_port);



	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the hash table - invalidated
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//for the victim
	//There should NOT be an entry in the MSHR for the victim
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(VICTIM_ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//verify state of the victim's client -- now the new request's client
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, victim_client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[victim_client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[victim_client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request() and handle_peer_and_manager_request():
    //! load miss; evicted in S;
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //!
    //! Since victim is in S state, it silently transits to I, and the new request
    //! continues:
    //! 1. C--->M, MESI_CM_I_to_S.
    void test_handle_peer_and_manager_request_load_eviction_l1_S_0()
    {
	//create a LOAD request
	const unsigned long ADDR = random();

	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;

	}

	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in S state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_S;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size()); // CM_I_to_S for the new request
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);



	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//for the victim
	//There should NOT be an entry in the MSHR for the victim
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(VICTIM_ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//verify state of the victim's client -- now the new request's client
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, victim_client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[victim_client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[victim_client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request() and handle_peer_and_manager_request():
    //! load miss; evicted in M;
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! 1. victim--->M, MESI_CM_M_to_I.
    //! 2. victim<---M, MESI_MC_GRANT_I.
    //! 3. victim--->M, MESI_CM_UNBLOCK_I.
    //!
    //! Now the new request is woken up.
    //! 1. C--->M, MESI_CM_I_to_S.
    void test_handle_peer_and_manager_request_load_eviction_l1_M_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;

	}

	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in M state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + 50;

        Coh_msg* req2 = new Coh_msg();
	req2->type = Coh_msg::COH_RPLY;
	req2->addr = VICTIM_ADDR;
	req2->msg = MESI_MC_GRANT_I;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + 50;


	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq, mreq1, mreq2;
	int l2_idx=0;
	int mux_idx=0;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    // CM_M_to_I, CM_UNBLOCK_I
	    mreq = mreqs[0];
	    mreq1 = mreqs[1];
	    l2_idx += 2;
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    mreq1 = *((Coh_msg*)(pkt1.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);

	    mux_idx += 2;
	}

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    //CM_I_to_S
	    mreq2 = mreqs[l2_idx];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();

	    NetworkPacket& pkt = pkts[mux_idx];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	}


	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq2.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq2.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq2.dst_port);


	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//for the victim
	//There should NOT be an entry in the MSHR for the victim
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(VICTIM_ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(VICTIM_ADDR));

	//verify state of the victim's client -- now the new request's client
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, victim_client->state);
	//CPPUNIT_ASSERT(creq == m_cachep->mcp_stalled_req[victim_client->getClientID()]);
	GenericPreqWrapper<Proc_req>* wrapper = dynamic_cast<GenericPreqWrapper<Proc_req>*>(
	 m_cachep->mcp_stalled_req[victim_client->getClientID()]->preqWrapper);
	CPPUNIT_ASSERT(creq == wrapper->preq);
    }



















//############
//Invalidation
//############

    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): DEMAND_I, L1 in E state.
    //!
    //! When manager needs to invalidate a client, it sends MESI_MC_DEMAND_I.
    //! If client is in E state, it sends MESI_CM_UNBLOCK_I, and enters I state.
    //!
    void test_handle_peer_and_manager_request_invalidation_l1_E_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	//manually put the ADDR in the hash table, and set the client state to E
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_E;

	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a MESI_MC_DEMAND_I
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_REQ;
	req->addr = ADDR;
	req->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets MESI_CM_UNBLOCK_I
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size());
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);


	//verify state of the MESI_L1_cache
	//There should not be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should not be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
	//There should not be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	CPPUNIT_ASSERT_EQUAL(MESI_C_I, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): DEMAND_I, L1 in M state.
    //!
    //! When manager needs to invalidate a client, it sends MESI_MC_DEMAND_I.
    //! If client is in M state, it sends MESI_CM_UNBLOCK_I_DIRTY, and enters I state.
    //!
    void test_handle_peer_and_manager_request_invalidation_l1_M_0()
    {
	const paddr_t ADDR = random();

	//manually put the ADDR in the hash table, and set the client state to E
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_M;

	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a MESI_MC_DEMAND_I
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_REQ;
	req->addr = ADDR;
	req->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets MESI_CM_UNBLOCK_I_DIRTY
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size());
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I_DIRTY, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);


	//verify state of the MESI_L1_cache
	//There should not be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should not be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
	//There should not be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	CPPUNIT_ASSERT_EQUAL(MESI_C_I, client->state);
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): DEMAND_I, L1 in S state.
    //!
    //! When manager needs to invalidate a client, it sends MESI_MC_DEMAND_I.
    //! If client is in S state, it sends MESI_CM_UNBLOCK_I, and enters I state.
    //!
    void test_handle_peer_and_manager_request_invalidation_l1_S_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	//manually put the ADDR in the hash table, and set the client state to E
	m_cachep->my_table->reserve_block_for(ADDR);
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);

	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_S;

	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a MESI_MC_DEMAND_I
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_REQ;
	req->addr = ADDR;
	req->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets MESI_CM_UNBLOCK_I
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size());
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);


	//verify state of the MESI_L1_cache
	//There should not be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should not be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
	//There should not be an entry in the stall buffer.
	CPPUNIT_ASSERT_EQUAL(0, (int)m_cachep->stalled_client_req_buffer.size());

	CPPUNIT_ASSERT_EQUAL(MESI_C_I, client->state);
    }
















//##########
//MSHR_STALL
//##########

    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request():
    //! no mshr entry available (i.e., C_MSHR_STALL).
    //!
    //! Create N requests to fill up the MSHR; then create one more request; it should
    //! have a C_MSHR_STALL.
    //!
    //! 1. C--->M, N MESI_CM_I_to_S.
    //! 2. Another request, which is stalled.
    //!
    //! Verify C_MSHR_STALL is created and put in the stall buffer.
    void test_handle_processor_request_MSHR_STALL_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();


	const paddr_t TAG_MASK = m_cachep->mshr->get_tag_mask();
	const int NUM_TAGS = m_cachep->mshr->assoc;
	assert(MSHR_SIZE == m_cachep->mshr->assoc);

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t addrs[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    addrs[i] = ad;

	}

	//create N LOAD requests
	Proc_req* crequests[NUM_TAGS];
	for(int i=0; i<NUM_TAGS; i++) {
	    crequests[i] = new Proc_req(0, NODE_ID, addrs[i], Proc_req::LOAD);
	}


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	for(int i=0; i<NUM_TAGS; i++) {
	    Manifold::Schedule(When, &MockProc::send_creq, m_procp, crequests[i]);
	}
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

	//create a LOAD request
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq); //this would C_MSHR_STALL
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	CPPUNIT_ASSERT_EQUAL(NUM_TAGS, (int)mux_pkts.size() + (int)l2_mreqs.size()); //N MESI_CM_I_to_S, the last one is stalled and didn't
	                                                   //send MESI_CM_I_to_S

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
	//There should be a MSHR_STALL in the stall buffer.
	std::list<L1_cache::Stall_buffer_entry>::reverse_iterator it = m_cachep->stalled_client_req_buffer.rbegin();
	CPPUNIT_ASSERT_EQUAL(C_MSHR_STALL, (*it).type);
    }



    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request():
    //! no mshr entry available (i.e., C_MSHR_STALL).
    //!
    //! Create N requests to fill up the MSHR; then create one more request; it should
    //! have a C_MSHR_STALL.
    //! Send a response to one of the previous N requests; this would free up an mshr entry.
    //! Verify the freed mshr entry is given to the latest request and it's freed from the
    //! stall buffer.
    //!
    //! 1. C--->M, N MESI_CM_I_to_S.
    //! 2. Another request, which is stalled.
    //! 3. C<---M, MESI_MC_GRANT_E_DATA.
    //! 4. C--->M, MESI_CM_I_to_E, or MESI_CM_I_to_S.
    //!
    //! Verify C_MSHR_STALL is woken up and removed from stall buffer.
    void test_handle_peer_and_manager_request_MSHR_STALL_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();


	const paddr_t TAG_MASK = m_cachep->mshr->get_tag_mask();
	const int NUM_TAGS = m_cachep->mshr->assoc;
	assert(MSHR_SIZE == m_cachep->mshr->assoc);

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t addrs[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    //m_cachep->my_table->reserve_block_for(ad);
	    addrs[i] = ad;

	}



	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	for(int i=0; i<NUM_TAGS; i++) {
	    Proc_req* crequest = new Proc_req(0, NODE_ID, addrs[i], Proc_req::LOAD);
	    Manifold::Schedule(When, &MockProc::send_creq, m_procp, crequest);
	}
	Ticks_t scheduledAt = When + MasterClock.NowTicks();
	When += PROC_CACHE + HT_LOOKUP + 10;

	//create a LOAD request
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq); //this would C_MSHR_STALL
	When += PROC_CACHE + HT_LOOKUP + 10;

	//randomly pick one request to reply to.
        Coh_msg* reply = new Coh_msg();
	reply->type = Coh_msg::COH_RPLY;
	const unsigned long REPLY_ADDR = addrs[random() % NUM_TAGS];
	reply->addr = REPLY_ADDR;
	reply->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(REPLY_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, reply);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *reply;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete reply;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

        //verify the downstream component gets a Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	CPPUNIT_ASSERT_EQUAL(NUM_TAGS+2, (int)mux_pkts.size() + (int)l2_mreqs.size()); //N MESI_CM_I_to_S, MESI_CM_UNBLOCK_S, MESI_CM_I_to_S

	//verify MockProc gets a response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	Proc_req& cresp = cresps[0];

	CPPUNIT_ASSERT_EQUAL(REPLY_ADDR, cresp.addr);

	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should NOT be a MSHR_STALL in the stall buffer.
	for(std::list<L1_cache::Stall_buffer_entry>::iterator it = m_cachep->stalled_client_req_buffer.begin();
	       it != m_cachep->stalled_client_req_buffer.end(); ++it) {
	    CPPUNIT_ASSERT(C_MSHR_STALL != (*it).type);
        }
    }


















//###############
//PREV_PEND_STALL
//###############

    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request():
    //! an mshr entry for the same line already exists (i.e., PREV_PEND_STALL).
    //!
    //! Send a request the L1; then send another with same address. The 2nd one
    //! should have a C_PREV_PEND_STALL.
    //!
    //! Verify C_PREV_PEND_STALL is created and put in the stall buffer.
    void test_handle_processor_request_PREV_PEND_STALL_0()
    {
	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + HT_LOOKUP + 10;

	//create another LOAD request with addr of same line
	paddr_t ADDR2 = m_cachep->my_table->get_line_addr(ADDR); //get the tag and index portion
	do {
	    ADDR2 |= random() % (0x1 << (m_cachep->my_table->get_offset_bits()));
	} while(ADDR2 == ADDR);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), m_cachep->my_table->get_line_addr(ADDR2));

	Proc_req* creq2 = new Proc_req(0, NODE_ID, ADDR2, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq2); //this would C_PREV_PEND_STALL
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should be a PREV_PEND_STALL in the stall buffer.
	std::list<L1_cache::Stall_buffer_entry>::reverse_iterator it = m_cachep->stalled_client_req_buffer.rbegin();
	CPPUNIT_ASSERT_EQUAL(C_PREV_PEND_STALL, (*it).type);
	CPPUNIT_ASSERT_EQUAL(ADDR2, (*it).req->addr);

    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): load request;
    //! an mshr entry for the same line already exists (i.e., PREV_PEND_STALL).
    //!
    //! Send a request to L1; then send another with same address. The 2nd one
    //! should have a C_PREV_PEND_STALL.
    //! Send a response to the first request; the stored request should be processed.
    //!
    //! Verify C_PREV_PEND_STALL is removed from the stall buffer.
    //!
    void test_handle_peer_and_manager_request_PREV_PEND_STALL_0()
    {
	Manifold::unhalt();
	Ticks_t When = 1;

	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + 10;

	//create another LOAD request with addr of same line
	unsigned long ADDR2 = m_cachep->my_table->get_line_addr(ADDR); //get the tag and index portion
	do {
	    ADDR2 |= random() % (0x1 << (m_cachep->my_table->get_offset_bits()));
	} while(ADDR2 == ADDR);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), m_cachep->my_table->get_line_addr(ADDR2));

	Proc_req* creq2 = new Proc_req(0, NODE_ID, ADDR2, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq2); //this would C_PREV_PEND_STALL
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a response to the 1st request
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();


        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size()); //MESI_CM_I_to_S, MESI_CM_UNBLOCK_E, both for 1st request
							//2nd request is a load hit, so it sends no messages.
	    mreq = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    mreq1 = *((Coh_msg*)(pkt1.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);
	}

	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets 2 response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	CPPUNIT_ASSERT_EQUAL(2, (int)cresps.size()); //for 2 requests

	CPPUNIT_ASSERT_EQUAL(ADDR, cresps[0].addr);
	CPPUNIT_ASSERT_EQUAL(ADDR2, cresps[1].addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR2)); //2nd request is hit, releases mshr
	//There should NOT be a PREV_PEND_STALL in the stall buffer.
	for(std::list<L1_cache::Stall_buffer_entry>::iterator it = m_cachep->stalled_client_req_buffer.begin();
	       it != m_cachep->stalled_client_req_buffer.end(); ++it) {
	    CPPUNIT_ASSERT(C_PREV_PEND_STALL != (*it).type);
        }
    }




    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): store request;
    //! an mshr entry for the same line already exists (i.e., PREV_PEND_STALL).
    //!
    //! Send a request to L1; then send a store request with same address. The 2nd one
    //! should have a C_PREV_PEND_STALL.
    //! Send a response to the first request; the stored request should be processed.
    //!
    //! Verify C_PREV_PEND_STALL is removed from the stall buffer.
    //!
    void test_handle_peer_and_manager_request_PREV_PEND_STALL_1()
    {
	Manifold::unhalt();
	Ticks_t When = 1;

	//create a LOAD request
	const unsigned long ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + HT_LOOKUP + 10;

	//create another LOAD request with addr of same line
	unsigned long ADDR2 = m_cachep->my_table->get_line_addr(ADDR); //get the tag and index portion
	do {
	    ADDR2 |= random() % (0x1 << (m_cachep->my_table->get_offset_bits()));
	} while(ADDR2 == ADDR);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), m_cachep->my_table->get_line_addr(ADDR2));

	Proc_req* creq2 = new Proc_req(0, NODE_ID, ADDR2, Proc_req::STORE); //2nd request is a store
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq2); //this would C_PREV_PEND_STALL
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a response to the 1st request
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();


        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size()); //MESI_CM_I_to_S, MESI_CM_UNBLOCK_E, both for 1st request
							//2nd request store hit, so it changes to M without sending
							//messages.
	    mreq = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    mreq1 = *((Coh_msg*)(pkt1.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);
	}

	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);


	//verify MockProc gets 2 response
        vector<Proc_req>& cresps = m_procp->get_cache_resps();
	CPPUNIT_ASSERT_EQUAL(2, (int)cresps.size()); //for 2 requests

	CPPUNIT_ASSERT_EQUAL(ADDR, cresps[0].addr);
	CPPUNIT_ASSERT_EQUAL(ADDR2, cresps[1].addr);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR2)); //2nd request is hit, releases mshr
	//There should NOT be a PREV_PEND_STALL in the stall buffer.
	for(std::list<L1_cache::Stall_buffer_entry>::iterator it = m_cachep->stalled_client_req_buffer.begin();
	       it != m_cachep->stalled_client_req_buffer.end(); ++it) {
	    CPPUNIT_ASSERT(C_PREV_PEND_STALL != (*it).type);
        }
    }






















//##############
//LRU_BUSY_STALL
//##############

    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): LRU_BUSY_STALL.
    //!
    //! Fill up a cache set; then send a request with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the evicted entry is in a transient state, the new request should have
    //! a LRU_BUSY_STALL.
    void test_handle_processor_request_LRU_BUSY_STALL_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t addrs[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);
	    addrs[i] = ad;
	}

	const paddr_t VICTIM_ADDR = addrs[0];



	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule NUM_TAGS requests to make the set full
	for(int i=0; i<NUM_TAGS; i++) {
	    Proc_req* creq = new Proc_req(0, NODE_ID, addrs[i], Proc_req::LOAD);
	    Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD); //this one would LRU_BUSY_STALL
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

	//verify state of the MESI_L1_cache
	//for the new request
	//There should NOT be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR)); //LRU_BUSY_STALLed request has given up its mshr
	//There should NOT be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));

	//There should be a LRU_BUSY_STALL in the stall buffer.
	std::list<L1_cache::Stall_buffer_entry>::reverse_iterator it = m_cachep->stalled_client_req_buffer.rbegin();
	CPPUNIT_ASSERT_EQUAL(C_LRU_BUSY_STALL, (*it).type);
	CPPUNIT_ASSERT_EQUAL(ADDR, (*it).req->addr);
    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): LRU_BUSY_STALL.
    //!
    //! Fill up a cache set; then send a request with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the evicted entry is in a transient state, the new request should have
    //! a LRU_BUSY_STALL.
    //! Send a response to allow the victim to finish; this would remove the LRU_BUSY_STALL
    //! from the stall buffer and process it.
    void test_handle_peer_and_manager_request_LRU_BUSY_STALL_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t addrs[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);
	    addrs[i] = ad;
	}

	const paddr_t VICTIM_ADDR = addrs[0];



	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule NUM_TAGS requests to make the set full
	for(int i=0; i<NUM_TAGS; i++) {
	    Proc_req* creq = new Proc_req(0, NODE_ID, addrs[i], Proc_req::LOAD);
	    Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD); //this one would LRU_BUSY_STALL
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a response to the 1st request (victim)
        Coh_msg* req = new Coh_msg();
	req->type = Coh_msg::COH_RPLY;
	req->addr = VICTIM_ADDR;
	req->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();

	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table, because it only started the eviction process
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));

	//There should NOT be a LRU_BUSY_STALL in the stall buffer.
	for(std::list<L1_cache::Stall_buffer_entry>::iterator it = m_cachep->stalled_client_req_buffer.begin();
	       it != m_cachep->stalled_client_req_buffer.end(); ++it) {
	    CPPUNIT_ASSERT(C_LRU_BUSY_STALL != (*it).type);
        }
    }























//###########
//TRANS_STALL
//###########


    //======================================================================
    //======================================================================
    //! @brief Test handle_processor_request(): TRANS_STALL: hit entry beying evicted.
    //!
    //! Fill up a cache set and put them in the E state, then send a request with a new
    //! tag for the same set. This causes the LRU entry to be evicted.
    //! While the victim is being evicted, send another request that hits on the victim.
    //! This request would have a C_TRANS_STALL.
    //! Verify a C_TRANS_STALL is created and put in the stall buffer.
    void test_handle_processor_request_TRANS_STALL_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t addrs[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);
	    m_cachep->my_table->reserve_block_for(ad);
	    addrs[i] = ad;
	}

	const paddr_t VICTIM_ADDR = addrs[0];

	//put the state of all the lines in the set to E
	for(int i=0; i<NUM_TAGS; i++) {
	    hash_entry* entry = m_cachep->my_table->get_entry(addrs[i]);
	    assert(entry);
	    MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	    client->state = MESI_C_E;
	}



	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a request which will miss and cause an eviction
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD); //this one would REPLACEMENT_STALL
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule another request which will hit on the victim
	//create another LOAD request with addr of same line
	paddr_t ADDR2 = m_cachep->my_table->get_line_addr(VICTIM_ADDR); //get the tag and index portion
	do {
	    ADDR2 |= random() % (0x1 << (m_cachep->my_table->get_offset_bits()));
	} while(ADDR2 == VICTIM_ADDR);

	Proc_req* creq2 = new Proc_req(0, NODE_ID, ADDR2, Proc_req::LOAD); //this one would TRANS_STALL
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq2);
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();



	//verify state of the MESI_L1_cache
	//for the new request
	//There should NOT be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR2)); //TRANS_STALLed request has given up its mshr

	//There should be a TRANS_STALL in the stall buffer.
	std::list<L1_cache::Stall_buffer_entry>::reverse_iterator it = m_cachep->stalled_client_req_buffer.rbegin();
	CPPUNIT_ASSERT_EQUAL(C_TRANS_STALL, (*it).type);
	CPPUNIT_ASSERT_EQUAL(ADDR2, (*it).req->addr);
    }





    //======================================================================
    //======================================================================
    //! @brief Test handle_peer_and_manager_request(): TRANS_STALL: hit entry beying evicted.
    //!
    //! Fill up a cache set and put them in the E state, then send a request with a new
    //! tag for the same set. This causes the LRU entry to be evicted.
    //! While the victim is being evicted, send another request that hits on the victim.
    //! This request would have a C_TRANS_STALL.
    //! Send a response to the victim's eviction request; this causes the victim to be evicted
    //! and the 1st request woken up.
    //! Sned a response to the 1st request; this causes the 2nd request to be woken up.
    //!
    //! Verify a C_TRANS_STALL is removed from the stall buffer and processed.
    void test_handle_peer_and_manager_request_TRANS_STALL_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t addrs[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);
	    m_cachep->my_table->reserve_block_for(ad);
	    addrs[i] = ad;
	}

	const paddr_t VICTIM_ADDR = addrs[0];

	//put the state of all the lines in the set to E
	for(int i=0; i<NUM_TAGS; i++) {
	    hash_entry* entry = m_cachep->my_table->get_entry(addrs[i]);
	    assert(entry);
	    MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	    client->state = MESI_C_E;
	}



	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a request which will miss and cause an eviction
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD); //this one would REPLACEMENT_STALL
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule another request which will hit on the victim
	//create another LOAD request with addr of same line
	paddr_t ADDR2 = m_cachep->my_table->get_line_addr(VICTIM_ADDR); //get the tag and index portion
	do {
	    ADDR2 |= random() % (0x1 << (m_cachep->my_table->get_offset_bits()));
	} while(ADDR2 == VICTIM_ADDR);

	Proc_req* creq2 = new Proc_req(0, NODE_ID, ADDR2, Proc_req::LOAD); //this one would TRANS_STALL
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq2);
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a response to the victim's eviction request
        Coh_msg* resp = new Coh_msg();
	resp->type = Coh_msg::COH_RPLY;
	resp->addr = VICTIM_ADDR;
	resp->msg = MESI_MC_GRANT_I;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, resp);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *resp;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete resp;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a response to the 1st request
        Coh_msg* resp2 = new Coh_msg();
	resp2->type = Coh_msg::COH_RPLY;
	resp2->addr = ADDR;
	resp2->msg = MESI_MC_GRANT_E_DATA;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, resp2);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *resp2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete resp2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;


	Manifold::StopAt(When);
	Manifold::Run();



	//verify state of the MESI_L1_cache
	//for the new request
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR2)); //TRANS_STALLed request has given up its mshr

	//There should NOT be a TRANS_STALL in the stall buffer.
	for(std::list<L1_cache::Stall_buffer_entry>::iterator it = m_cachep->stalled_client_req_buffer.begin();
	       it != m_cachep->stalled_client_req_buffer.end(); ++it) {
	    CPPUNIT_ASSERT(C_TRANS_STALL != (*it).type);
        }
    }





















//####################
//Respond with default
//####################

    //======================================================================
    //======================================================================
    //! @brief Test respond_with_default().
    //!
    //! Client receives a MESI_MC_DEMAND_I, but there's no mshr or hash table entry
    //! for the address. This happens because when client goes from S to I when it's
    //! being evicted, it doesn't notify the manager, so the manager still thinks it's
    //! in S state.
    //!
    //! 1. M--->C, MESI_MC_DEMAND_I
    //! 2. M<---C, MESI_MC_UNBLOCK_I
    //!
    void test_respond_with_default_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();


	Manifold::unhalt();
	Ticks_t When = 1;

	//schedule a MESI_MC_DEMAND_I
	Coh_msg* req = new Coh_msg();
	req->type = Coh_msg :: COH_REQ;
	req->addr = ADDR;
	req->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();


        //verify the downstream component gets a Coh_mem_req.
	Coh_msg mreq;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(1, (int)mreqs.size());
	    mreq = mreqs[0];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(1, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(ADDR, mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table.
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
    }




    //======================================================================
    //======================================================================
    //! @brief Test respond_with_default().
    //!
    //! Client receives a MESI_MC_DEMAND_I, there is an MSHR but no hash entry.
    //! This happens when the address was previously in S state and was evicted.
    //! Since it's in S, it didn't notify manager of the eviction. Later on, if
    //! there's another request for the address, an MSHR entry is allocated. If
    //! at this point, the line is being evicted, since manager still thinks client
    //! is a sharer, a DEMAND_I is sent. Client need respond with UNBLOCK_I.
    //!
    //! 1. processor --> C, read request; an MSHR entry is allocated.
    //! 2. M--->C, MESI_MC_DEMAND_I
    //! 3. M<---C, MESI_CM_UNBLOCK_I
    //!
    void test_respond_with_default_1()
    {

	//Fill up a set.
	const paddr_t ADDR = random();

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the state of all the lines in the set to E
	for(int i=0; i<NUM_TAGS; i++) {
	    hash_entry* entry = m_cachep->my_table->get_entry(set_entries[i]);
	    assert(entry);
	    MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	    client->state = MESI_C_E;
	}



	Manifold::unhalt();
	Ticks_t When = 1;


	//create a LOAD request that causes eviction: there is MSHR but no hash entry for ADDR
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);

	When += PROC_CACHE + HT_LOOKUP + 10;

	//schedule a MESI_MC_DEMAND_I
	Coh_msg* req = new Coh_msg();
	req->type = Coh_msg :: COH_REQ;
	req->addr = ADDR;
	req->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, req);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *req;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete req;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}
	When += PROC_CACHE + HT_LOOKUP + 10;

	Manifold::StopAt(When);
	Manifold::Run();


        //verify the downstream component gets CM_E_to_I and UNBLOCK_I
	Coh_msg mreq, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mreqs.size()); //MESI_CM_I_to_S, MESI_CM_UNBLOCK_E, both for 1st request
							//2nd request store hit, so it changes to M without sending
							//messages.
	    mreq = mreqs[0];
	    mreq1 = mreqs[1];
	}
	else {
	    vector<NetworkPacket>& pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)pkts.size());

	    NetworkPacket& pkt = pkts[0];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    NetworkPacket& pkt1 = pkts[1];
	    mreq1 = *((Coh_msg*)(pkt1.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt1.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt1.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt1.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt1.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt1.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt1.data_size);
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	CPPUNIT_ASSERT_EQUAL(ADDR, mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should NOT be an entry in the hash table.
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->my_table->has_match(ADDR));
    }























//##################
//request race
//##################

    //======================================================================
    //======================================================================
    //! @brief Test request race: client in EI, receives FWD_E.
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the client is in E, it sends MESI_CM_E_to_I to manager. Before
    //! the eviction completes, manager sends a FWD_E request, which is processed.
    //! Client enters I, and wakes up the waiting request.
    //!
    //! 1. Fill up a set, put LRU in E state. Make a read request to the same set.
    //!    This causes eviction of LRU.
    //! 2. Client --> manager, CM_E_to_I. Client enters EI.
    //! 3. Manager --> client, MC_FWD_E.
    //! 4. Client --> requestor, CC_E_DATA.
    //! 5. Eviction completes. Original request is processed. Client --> manager, CM_I_to_S.
    //!
    void test_request_race_owner_EI_recv_FWD_E_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in E state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in EI now; before the eviction is completed, send MC_FWD_E to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = VICTIM_ADDR;
	umreq->msg = MESI_MC_FWD_E;
	int FWDID;
	while((FWDID = random() % 1024) == NODE_ID);
	umreq->forward_id = FWDID;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();




	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size() + (int)l2_mreqs.size());

        int mux_idx = 0;
	int l2_idx = 0;

	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;
	}

	//CC_E_DATA can only go to the MUX.
	{
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(FWDID, pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_E_DATA, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL(FWDID, mreq.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq.dst_port);
	    mux_idx++;
	}

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	}
    }




    //======================================================================
    //======================================================================
    //! @brief Test request race: client in EI, receives FWD_S.
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the client is in E, it sends MESI_CM_E_to_I to manager. Before
    //! the eviction completes, manager sends a FWD_S request, which is processed.
    //! Client enters I, and wakes up the waiting request.
    //!
    //! 1. Fill up a set, put LRU in E state. Make a read request to the same set.
    //!    This causes eviction of LRU.
    //! 2. Client --> manager, CM_E_to_I. Client enters EI.
    //! 3. Manager --> client, MC_FWD_S.
    //! 4. Client --> requestor, CC_E_DATA.
    //! 6. Eviction completes. Original request is processed. Client --> manager, CM_I_to_S.
    //!
    void test_request_race_owner_EI_recv_FWD_S_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in E state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in EI now; before the eviction is completed, send MC_FWD_S to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = VICTIM_ADDR;
	umreq->msg = MESI_MC_FWD_S;
	int FWDID;
	while((FWDID = random() % 1024) == NODE_ID);
	umreq->forward_id = FWDID;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();




	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size() + (int)l2_mreqs.size());

        int mux_idx = 0;
	int l2_idx = 0;

	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;
	}

	//CC_E_DATA can only go to the MUX.
	{
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(FWDID, pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_E_DATA, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL(FWDID, mreq.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq.dst_port);
	    mux_idx++;
	}

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	}
    }



    //======================================================================
    //======================================================================
    //! @brief Test request race: client in MI, receives FWD_E.
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the client is in M, it sends MESI_CM_M_to_I to manager. Before
    //! the eviction completes, manager sends a FWD_E request, which is processed.
    //! Client enters I, and wakes up the waiting request.
    //!
    //! 1. Fill up a set, put LRU in E state. Make a read request to the same set.
    //!    This causes eviction of LRU.
    //! 2. Client --> manager, CM_M_to_I. Client enters MI.
    //! 3. Manager --> client, MC_FWD_E.
    //! 4. Client --> requestor, CC_M_DATA.
    //! 5. Eviction completes. Original request is processed. Client --> manager, CM_I_to_S.
    //!
    void test_request_race_owner_MI_recv_FWD_E_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in M state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in MI now; before the eviction is completed, send MC_FWD_E to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = VICTIM_ADDR;
	umreq->msg = MESI_MC_FWD_E;
	int FWDID;
	while((FWDID = random() % 1024) == NODE_ID);
	umreq->forward_id = FWDID;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();




	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_mem_req.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size() + (int)l2_mreqs.size());

        int mux_idx = 0;
	int l2_idx = 0;

	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;
	}

	//CC_E_DATA can only go to the MUX.
	{
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(FWDID, pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_M_DATA, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL(FWDID, mreq.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq.dst_port);
	    mux_idx++;
	}

	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	}
    }




    //======================================================================
    //======================================================================
    //! @brief Test request race: client in MI, receives FWD_S.
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the client is in M, it sends MESI_CM_M_to_I to manager. Before
    //! the eviction completes, manager sends a FWD_S request, which is processed.
    //! Client enters I, and wakes up the waiting request.
    //!
    //! 1. Fill up a set, put LRU in E state. Make a read request to the same set.
    //!    This causes eviction of LRU.
    //! 2. Client --> manager, CM_M_to_I. Client enters MI.
    //! 3. Manager --> client, MC_FWD_S.
    //! 4. Client --> requestor, CC_S_DATA. //note it's CC_S_DATA, not CC_M_DATA.
    //! 5. Client --> manager, CM_WRITEBACK.
    //! 6. Eviction completes. Original request is processed. Client --> manager, CM_I_to_S.
    //!
    void test_request_race_owner_MI_recv_FWD_S_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in M state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in MI now; before the eviction is completed, send MC_FWD_S to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = VICTIM_ADDR;
	umreq->msg = MESI_MC_FWD_S;
	int FWDID;
	while((FWDID = random() % 1024) == NODE_ID);
	umreq->forward_id = FWDID;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();




	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_msg.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size() + (int)l2_mreqs.size());

        int mux_idx = 0;
	int l2_idx = 0;

	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;

	}

	//CC_E_DATA can only go to the MUX.
	{
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(FWDID, pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CC_M_DATA, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL(FWDID, mreq.dst_id);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, mreq.dst_port);
	    mux_idx++;
	}


	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	}
    }




    //======================================================================
    //======================================================================
    //! @brief Test request race: client in EI, receives DEMAND_I.
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the client is in E, it sends MESI_CM_E_to_I to manager. Before
    //! the eviction completes, manager sends a DEMAND_I request, which is processed.
    //! Client enters I, and wakes up the waiting request.
    //!
    //! 1. Fill up a set, put LRU in E state. Make a read request to the same set.
    //!    This causes eviction of LRU.
    //! 2. Client --> manager, CM_E_to_I. Client enters EI.
    //! 3. Manager --> client, MC_DEMAND_I.
    //! 4. Client --> manager, CM_UNBLOCK_I.
    //! 5. Eviction completes. Original request is processed. Client --> manager, CM_I_to_S.
    //!
    void test_request_race_owner_EI_recv_DEMAND_I_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in E state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_E;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in EI now; before the eviction is completed, send MC_DEMAND_I to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = VICTIM_ADDR;
	umreq->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();




	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_msg.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size() + (int)l2_mreqs.size());

        int mux_idx = 0;
	int l2_idx = 0;

	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_E_to_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;

	    pkt = mux_pkts[mux_idx];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;
	}


	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	}
    }




    //======================================================================
    //======================================================================
    //! @brief Test request race: client in MI, receives DEMAND_I.
    //!
    //! Fill up a cache set; then LOAD with a new tag for the same set. This
    //! causes the LRU entry to be evicted.
    //! Since the client is in M, it sends MESI_CM_M_to_I to manager. Before
    //! the eviction completes, manager sends a DEMAND_I request, which is processed.
    //! Client enters I, and wakes up the waiting request.
    //!
    //! 1. Fill up a set, put LRU in M state. Make a read request to the same set.
    //!    This causes eviction of LRU.
    //! 2. Client --> manager, CM_M_to_I. Client enters MI.
    //! 3. Manager --> client, MC_DEMAND_I.
    //! 4. Client --> manager, CM_UNBLOCK_I_DIRTY.
    //! 5. Eviction completes. Original request is processed. Client --> manager, CM_I_to_S.
    //!
    void test_request_race_owner_MI_recv_DEMAND_I_0()
    {
	//create a LOAD request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::LOAD);

	const paddr_t TAG_MASK = m_cachep->my_table->get_tag_mask();
	const int NUM_TAGS = m_cachep->my_table->assoc;

	//create NUM_TAGS unique tags
	set<paddr_t> addrset; //use a set to generate unique values

	paddr_t set_entries[NUM_TAGS];

	for(int i=0; i<NUM_TAGS; i++) {
	    paddr_t ad;
	    pair<set<paddr_t>::iterator,bool> ret; //return value; 2nd of the pair is false if the
						       //value is already in the set
	    //make sure ad is new and different from ADDR
	    do {
		ad = ADDR & (~TAG_MASK); //clear the tag portion
		ad |= (random() & TAG_MASK); // OR-in a random tag
		if(ad != ADDR)
		    ret = addrset.insert(ad);
	    } while(ad == ADDR || ret.second == false);

	    m_cachep->my_table->reserve_block_for(ad);
	    set_entries[i] = ad;
	}
	//The set is full.

	const paddr_t VICTIM_ADDR = set_entries[0];

	//put the victim's client in E state
	hash_entry* victim_entry = m_cachep->my_table->get_entry(VICTIM_ADDR);
	assert(victim_entry);
	MESI_client* victim_client = dynamic_cast<MESI_client*>(m_cachep->clients[victim_entry->get_idx()]);
	victim_client->state = MESI_C_M;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in MI now; before the eviction is completed, send MC_DEMAND_I to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = VICTIM_ADDR;
	umreq->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();




	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR)); //invalidated!


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_msg.
        vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
        vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();

	CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size() + (int)l2_mreqs.size());

        int mux_idx = 0;
	int l2_idx = 0;

	if(m_l2map->lookup(VICTIM_ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I_DIRTY, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	    l2_idx++;
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_M_to_I, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;

	    pkt = mux_pkts[mux_idx];
	    mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(VICTIM_ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(VICTIM_ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I_DIRTY, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	    mux_idx++;
	}


	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), l2_mreqs[l2_idx].addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, l2_mreqs[l2_idx].msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, l2_mreqs[l2_idx].dst_port);
	}
	else {
	    NetworkPacket& pkt = mux_pkts[mux_idx];
	    Coh_msg mreq = *((Coh_msg*)(pkt.data));

	    CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
	    CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
	    CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
	    CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);

	    CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	    CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_S, mreq.msg);
	    CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);
	}
    }



    //======================================================================
    //======================================================================
    //! @brief Test request race: client in SE, receives DEMAND_I.
    //!
    //! Put a tag in the hash table, set client's state to S. Send a write request
    //! to client, which sends I_to_E to manager and enters SE state.
    //! At this point, send DEMAND_I to client. Client replies with UNBLOCK_I.
    //!
    //! 1. Client --> manager, CM_I_to_E. Client enters SE.
    //! 2. Manager --> client, MC_DEMAND_I.
    //! 3. Client --> manager, CM_UNBLOCK_I.
    //!
    void test_request_race_client_SE_recv_DEMAND_I_0()
    {
	//create a request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//put ADDR in hash table.
	m_cachep->my_table->reserve_block_for(ADDR);

	//put the client in S state
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_S;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in SE now; send MC_DEMAND_I to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = ADDR;
	umreq->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));


	//verify state of the client
	entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_msg.
	Coh_msg mreq, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)l2_mreqs.size());
	    mreq = l2_mreqs[0];
	    mreq1 = l2_mreqs[1];
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mux_pkts.size());

	    for(int m=0; m<2; m++) {
		NetworkPacket& pkt = mux_pkts[m];
		CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
		CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
		CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
		CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
		CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
		CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	    }
	    mreq = *((Coh_msg*)(mux_pkts[0].data));
	    mreq1 = *((Coh_msg*)(mux_pkts[1].data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

    }




    //======================================================================
    //======================================================================
    //! @brief Test request race: client in SE, receives DEMAND_I.
    //!
    //! Put a tag in the hash table, set client's state to S. Send a write request
    //! to client, which sends I_to_E to manager and enters SE state.
    //! At this point, send DEMAND_I to client. Client replies with UNBLOCK_I.
    //! Finally, client receives CC_M_DATA, and replies to manager with UNBLOCK_E.
    //!
    //! 1. Client --> manager, CM_I_to_E. Client enters SE.
    //! 2. Manager --> client, MC_DEMAND_I.
    //! 3. Client --> manager, CM_UNBLOCK_I.
    //! 4. Owner --> client, CC_M_DATA.
    //! 5. Client --> manager, UNBLOCK_E.
    //!
    void test_request_race_client_SE_recv_DEMAND_I_1()
    {
	//create a request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//put ADDR in hash table.
	m_cachep->my_table->reserve_block_for(ADDR);

	//put the client in S state
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	client->state = MESI_C_S;


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in SE now; send MC_DEMAND_I to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = ADDR;
	umreq->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	//the client still in SE; send CC_M_DATA to it.
	{
	    Coh_msg* umreq2 = new Coh_msg();
	    umreq2->type = Coh_msg::COH_RPLY; //CC_M_DATA is a reply !!!
	    umreq2->addr = ADDR;
	    umreq2->msg = MESI_CC_M_DATA;

	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq2;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq2;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should NOT be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(false, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));


	//verify state of the client
	entry = m_cachep->my_table->get_entry(ADDR);
	CPPUNIT_ASSERT(entry != 0);
	CPPUNIT_ASSERT_EQUAL(MESI_C_M, client->state);

        //verify the downstream component gets Coh_mem_req.
	Coh_msg mreq, mreq1, mreq2;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(3, (int)l2_mreqs.size());
	    mreq = l2_mreqs[0];
	    mreq1 = l2_mreqs[1];
	    mreq2 = l2_mreqs[2];
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(3, (int)mux_pkts.size());

	    for(int m=0; m<3; m++) {
		NetworkPacket& pkt = mux_pkts[m];
		CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
		CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
		CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
		CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
		CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
		CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	    }
	    mreq = *((Coh_msg*)(mux_pkts[0].data));
	    mreq1 = *((Coh_msg*)(mux_pkts[1].data));
	    mreq2 = *((Coh_msg*)(mux_pkts[2].data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq2.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_E, mreq2.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq2.dst_port);

    }




    //======================================================================
    //======================================================================
    //! @brief Test request race: client in IE, receives DEMAND_I.
    //!
    //! Send a read or write request to cache; client sends I_to_E to manager
    //! and enters IE state.
    //! At this point, send DEMAND_I to client. Client replies with UNBLOCK_I.
    //!
    //! 1. Client --> manager, CM_I_to_E. Client enters IE.
    //! 2. Manager --> client, MC_DEMAND_I.
    //! 3. Client --> manager, CM_UNBLOCK_I.
    //!
    void test_request_race_client_IE_recv_DEMAND_I_0()
    {
	//create a request
	const paddr_t ADDR = random();
	Proc_req* creq = new Proc_req(0, NODE_ID, ADDR, Proc_req::STORE);

	//verify ADDR not in hash table.
	CPPUNIT_ASSERT(m_cachep->my_table->has_match(ADDR) == false);


	Manifold::unhalt();
	Ticks_t When = 1;
	//schedule for the MockProc to send the cache_req
	Manifold::Schedule(When, &MockProc::send_creq, m_procp, creq);
	Ticks_t scheduledAt = When + MasterClock.NowTicks();

	When += PROC_CACHE + 50;

	//the client is in IE now; send MC_DEMAND_I to it.
        Coh_msg* umreq = new Coh_msg();
	umreq->type = Coh_msg::COH_REQ;
	umreq->addr = ADDR;
	umreq->msg = MESI_MC_DEMAND_I;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    Manifold::Schedule(When, &MockLower::send_MESI, m_l2p, umreq);
	}
	else {
	    NetworkPacket* mgrPkt = new NetworkPacket;
	    mgrPkt->type = m_cachep->COH_MSG;
	    mgrPkt->dst = NODE_ID;
	    *((Coh_msg*)(mgrPkt->data)) = *umreq;
	    mgrPkt->data_size = sizeof(Coh_msg);
	    delete umreq;

	    Manifold::Schedule(When, &MockMux::send_pkt, m_muxp, mgrPkt);
	}

	When += PROC_CACHE + 50;

	Manifold::StopAt(When);
	Manifold::Run();


	//verify state of the MESI_L1_cache
	//There should be an entry in the MSHR for the original request.
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->mshr->has_match(ADDR));
	//There should be an entry in the hash table
	CPPUNIT_ASSERT_EQUAL(true, m_cachep->my_table->has_match(ADDR));


	//verify state of the client
	hash_entry* entry = m_cachep->my_table->get_entry(ADDR);
	assert(entry);
	MESI_client* client = dynamic_cast<MESI_client*>(m_cachep->clients[entry->get_idx()]);
	CPPUNIT_ASSERT_EQUAL(MESI_C_IE, client->state);

        //verify the downstream component gets Coh_mem_req.
	Coh_msg mreq, mreq1;
	if(m_l2map->lookup(ADDR) == NODE_ID) {
	    vector<Coh_msg>& l2_mreqs = m_l2p->get_mem_reqs();
	    CPPUNIT_ASSERT_EQUAL(2, (int)l2_mreqs.size());
	    mreq = l2_mreqs[0];
	    mreq1 = l2_mreqs[1];
	} else {
	    vector<NetworkPacket>& mux_pkts = m_muxp->get_pkts();
	    CPPUNIT_ASSERT_EQUAL(2, (int)mux_pkts.size());

	    for(int m=0; m<2; m++) {
		NetworkPacket& pkt = mux_pkts[m];
		CPPUNIT_ASSERT_EQUAL(m_cachep->COH_MSG, pkt.type);
		CPPUNIT_ASSERT_EQUAL(NODE_ID, pkt.src);
		CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLP_ID, pkt.src_port);
		CPPUNIT_ASSERT_EQUAL(m_l2map->lookup(ADDR), pkt.dst);
		CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, pkt.dst_port);
		CPPUNIT_ASSERT_EQUAL((int)sizeof(Coh_msg), pkt.data_size);
	    }
	    mreq = *((Coh_msg*)(mux_pkts[0].data));
	    mreq1 = *((Coh_msg*)(mux_pkts[1].data));
	}

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_I_to_E, mreq.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq.dst_port);

	CPPUNIT_ASSERT_EQUAL(m_cachep->my_table->get_line_addr(ADDR), mreq1.addr);
	CPPUNIT_ASSERT_EQUAL((int)MESI_CM_UNBLOCK_I, mreq1.msg);
	CPPUNIT_ASSERT_EQUAL((int)LLP_cache :: LLS_ID, mreq1.dst_port);

    }


    //! Build a test suite.
    static CppUnit::Test* suite()
    {
	CppUnit::TestSuite* mySuite = new CppUnit::TestSuite("MESI_LLP_cacheTest");
	/*
	*/
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_load_l1_I_0", &MESI_LLP_cacheTest::test_handle_processor_request_load_l1_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_load_l1_ESM_0", &MESI_LLP_cacheTest::test_handle_processor_request_load_l1_ESM_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_store_l1_I_0", &MESI_LLP_cacheTest::test_handle_processor_request_store_l1_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_store_l1_M_0", &MESI_LLP_cacheTest::test_handle_processor_request_store_l1_M_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_store_l1_E_0", &MESI_LLP_cacheTest::test_handle_processor_request_store_l1_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_store_l1_S_0", &MESI_LLP_cacheTest::test_handle_processor_request_store_l1_S_0));
        //response
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_l1_I_l2_E_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_l1_I_l2_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_l1_I_l2_E_owner_M_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_l1_I_l2_E_owner_M_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_l1_I_l2_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_l1_I_l2_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_l1_I_l2_I_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_l1_I_l2_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_store_l1_I_l2_E_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_store_l1_I_l2_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_store_l1_I_l2_E_owner_M_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_store_l1_I_l2_E_owner_M_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_store_l1_I_l2_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_store_l1_I_l2_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_store_l1_I_l2_I_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_store_l1_I_l2_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_store_hit_l1_S_l2_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_store_hit_l1_S_l2_S_0));
	//owner behavior
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_owner_E_recv_FWD_E_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_owner_E_recv_FWD_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_owner_M_recv_FWD_E_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_owner_M_recv_FWD_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_owner_E_recv_FWD_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_owner_E_recv_FWD_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_owner_M_recv_FWD_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_owner_M_recv_FWD_S_0));
	//eviction
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_load_eviction_l1_E_0", &MESI_LLP_cacheTest::test_handle_processor_request_load_eviction_l1_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_load_eviction_l1_S_0", &MESI_LLP_cacheTest::test_handle_processor_request_load_eviction_l1_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_load_eviction_l1_M_0", &MESI_LLP_cacheTest::test_handle_processor_request_load_eviction_l1_M_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_eviction_l1_E_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_eviction_l1_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_eviction_l1_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_eviction_l1_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_load_eviction_l1_M_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_load_eviction_l1_M_0));
	//invalidation
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_invalidation_l1_E_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_invalidation_l1_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_invalidation_l1_M_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_invalidation_l1_M_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_invalidation_l1_S_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_invalidation_l1_S_0));
	//MSHR_STALL
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_MSHR_STALL_0", &MESI_LLP_cacheTest::test_handle_processor_request_MSHR_STALL_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_MSHR_STALL_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_MSHR_STALL_0));
	//PREV_PEND_STALL
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_PREV_PEND_STALL_0", &MESI_LLP_cacheTest::test_handle_processor_request_PREV_PEND_STALL_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_PREV_PEND_STALL_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_PREV_PEND_STALL_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_PREV_PEND_STALL_1", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_PREV_PEND_STALL_1));
	//LRU_BUSY_STALL
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_LRU_BUSY_STALL_0", &MESI_LLP_cacheTest::test_handle_processor_request_LRU_BUSY_STALL_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_LRU_BUSY_STALL_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_LRU_BUSY_STALL_0));
	//TRANS_STALL, e.g., the hit entry is being evicted
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_processor_request_TRANS_STALL_0", &MESI_LLP_cacheTest::test_handle_processor_request_TRANS_STALL_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_handle_peer_and_manager_request_TRANS_STALL_0", &MESI_LLP_cacheTest::test_handle_peer_and_manager_request_TRANS_STALL_0));
	//Respond with default
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_respond_with_default_0", &MESI_LLP_cacheTest::test_respond_with_default_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_respond_with_default_1", &MESI_LLP_cacheTest::test_respond_with_default_1));
	//Request race. A request to manager is being processed, and client receives a request
	//for the same line from manager. There are 8 cases.
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_owner_EI_recv_FWD_E_0", &MESI_LLP_cacheTest::test_request_race_owner_EI_recv_FWD_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_owner_EI_recv_FWD_S_0", &MESI_LLP_cacheTest::test_request_race_owner_EI_recv_FWD_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_owner_MI_recv_FWD_E_0", &MESI_LLP_cacheTest::test_request_race_owner_MI_recv_FWD_E_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_owner_MI_recv_FWD_S_0", &MESI_LLP_cacheTest::test_request_race_owner_MI_recv_FWD_S_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_owner_EI_recv_DEMAND_I_0", &MESI_LLP_cacheTest::test_request_race_owner_EI_recv_DEMAND_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_owner_MI_recv_DEMAND_I_0", &MESI_LLP_cacheTest::test_request_race_owner_MI_recv_DEMAND_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_client_SE_recv_DEMAND_I_0", &MESI_LLP_cacheTest::test_request_race_client_SE_recv_DEMAND_I_0));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_client_SE_recv_DEMAND_I_1", &MESI_LLP_cacheTest::test_request_race_client_SE_recv_DEMAND_I_1));
	mySuite->addTest(new CppUnit::TestCaller<MESI_LLP_cacheTest>("test_request_race_client_IE_recv_DEMAND_I_0", &MESI_LLP_cacheTest::test_request_race_client_IE_recv_DEMAND_I_0));
	/*
	*/



	return mySuite;
    }
};


Clock MESI_LLP_cacheTest :: MasterClock(MESI_LLP_cacheTest :: MASTER_CLOCK_HZ);

int main()
{
    Manifold :: Init();
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( MESI_LLP_cacheTest::suite() );
    if(runner.run("", false))
	return 0; //all is well
    else
	return 1;

}


