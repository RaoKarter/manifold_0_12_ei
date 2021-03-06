//!
//! @brief This program is a component test (as opposed to unit test) program,
//! since it involves multiple classes.
//! We create two components c0, c1 for two different LPs and set up the connection
//! between the components. For each component, a few events are scheduled.
//! c0's handler for the events causes messages to be sent to c1. On the other hand,
//! c1's handler for the events does nothing. When messages are received by c1,
//! new events are automatically scheduled. c1 uses a different handler to handle
//! those events.
//!
//! Classes involved include: Manifold, Component, LinkBase, Link,
//! LinkOutputBase, LinkOutput, LinkOutputRemote, LinkInputBase, LinkInputBaseT,
//! LinkInput, Messenger, Message_s.
//!
//! In this program, the components are time based. No clock is defined.
//! This program uses a template class, so it can be used to test all primitive types
//! that are sent between two components.
//!
#include <TestFixture.h>
#include <TestAssert.h>
#include <TestSuite.h>
#include <Test.h>
#include <TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <stdlib.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "mpi.h"
#include "messenger.h"
#include "component.h"
#include "manifold.h"
#include "link.h"

using namespace std;
using namespace manifold::kernel;


//comment the following line if debug info is not desired.
#define DBG

int Mytid; //task id

//####################################################################
// helper classes
//####################################################################
template<typename T>
class MyObj1 : public Component {
    private:
	vector<Time_t> m_recvTime;

	vector<T> m_incoming;

	#ifdef DBG
	ofstream&  m_dbg_log;
	#endif
    public:
        static const int C0_OUT = 2;
        static const int C1_IN = 5;
        static const Time_t C0_OUT_LATENCY = 0.0123;

	#ifdef DBG
        MyObj1(const ofstream & dbg_log) : m_dbg_log(const_cast<ofstream&>(dbg_log)) {}
	#endif

        vector<Time_t>& getRecvTime() { return m_recvTime; }

        vector<T>& getIncoming() { return m_incoming; }

	//! Handler for initially scheduled events.
	//! Note that only task 0 sends out messages.
	void handler(T data)
	{
	    #ifdef DBG
	    Time_t now = Manifold :: Now();

	    m_dbg_log << "@@@ " << Manifold :: Now() << endl;
	    #endif

            if(Mytid == 0) { //c0 sends messages to c1
	        Send(C0_OUT, data);
		#ifdef DBG
		m_dbg_log << "    handler called "<< endl;
		m_dbg_log << "    sendTime= " << now
			  << " recvTime= " << now + C0_OUT_LATENCY
			  << " data= " << data
			  << endl;
		#endif
	    }
	}

        //! Handler for incoming message.
	void process_incoming(int inputIndex, T data)
	{
	    #ifdef DBG
	    m_dbg_log << "@@@ " << Manifold :: Now() << endl;
	    m_dbg_log << "    process_incoming  "
	              << " inputIdex= " << inputIndex
	              << " data= " << data
		      << endl;
	    #endif
	    // save the received data and time for verification later.
            m_incoming.push_back(data);
	    m_recvTime.push_back(Manifold :: Now());
	}
};


//####################################################################
//####################################################################
class MessagingTest : public CppUnit::TestFixture {
    private:

        static const double DOUBLE_COMP_DELTA = 1.0E-5;

    public:
        //!
	//! Initialization function. Inherited from the CPPUnit framework.
        //!
        void setUp()
	{
	}

        template<typename T>
        void test_messaging()
	{
	    Manifold::Reset(Manifold::TIMED, SyncAlg::SA_CMB);

	    MPI_Comm_rank(MPI_COMM_WORLD, &Mytid);

	    #ifdef DBG
	    // create a file into which to write debug info.
	    char buf[10];
	    sprintf(buf, "DBG_LOG%d", Mytid);
	    ofstream DBG_LOG(buf);
	       //redirect cout to file.
		std::streambuf* cout_sbuf = std::cout.rdbuf(); // save original sbuf
		std::cout.rdbuf(DBG_LOG.rdbuf()); // redirect cout
	    #endif

	    const int SIZE=5; // number of output messages to send from c0 to c1.

	    struct timeval ts;
	    gettimeofday(&ts, NULL);
	    srandom(ts.tv_usec + 1234 * Mytid);

	    Time_t when[SIZE]; // when output occurs
	    when[0] = (random()/(RAND_MAX+1.0) * 10);  //a number between 0 and 10
	    for(int i=1; i<SIZE; i++) {
		//when[i] = when[i-1] + d;   1 <= d < 6
		when[i] = when[i-1] + (random()/(RAND_MAX+1.0) * 5 + 1);
	    }

	    #ifdef DBG
            DBG_LOG << "Events scheduled at: ";
	    for(int i=0; i<SIZE; i++) {
		DBG_LOG << when[i] << ", ";
	    }
	    DBG_LOG << endl;
	    DBG_LOG << "Link delay= " << MyObj1<T>::C0_OUT_LATENCY << endl;
	    #endif


	    #ifdef DBG
	    CompId_t c0 = Component :: Create<MyObj1<T> >(0, DBG_LOG); //c0 created for LP 0
	    CompId_t c1 = Component :: Create<MyObj1<T> >(1, DBG_LOG); //c1 created for LP 1
	    #else
	    CompId_t c0 = Component :: Create<MyObj1<T> >(0); //c0 created for LP 0
	    CompId_t c1 = Component :: Create<MyObj1<T> >(1); //c1 created for LP 1
	    #endif

	    MyObj1<T>* c0p = Component :: GetComponent<MyObj1<T> >(c0);
	    MyObj1<T>* c1p = Component :: GetComponent<MyObj1<T> >(c1);

	    //connect the two: two-way connection even though C1 never sends to C0
	    //if C0 has no input link, then CMB cannot be used because it's always safe
	    //to process events and it will never send null messages.
	    Manifold :: ConnectTime(c0, MyObj1<T>::C0_OUT, c1, MyObj1<T>::C1_IN, &MyObj1<T>::process_incoming, MyObj1<T>::C0_OUT_LATENCY);
	    Manifold :: ConnectTime(c1, MyObj1<T>::C1_IN, c0, MyObj1<T>::C0_OUT, &MyObj1<T>::process_incoming, MyObj1<T>::C0_OUT_LATENCY);

	    MyObj1<T>* comp = (Mytid == 0) ? c0p : c1p;


	    T send_data[SIZE];
	    // schedule a few events
	    for(int i=0; i<SIZE; i++) {
		send_data[i] = (T)(random() & 0xffffffff);
		Manifold :: ScheduleTime(when[i], &MyObj1<T>::handler, comp, send_data[i]);
	    }
	    const unsigned int STOP = 1000;
	    CPPUNIT_ASSERT(when[SIZE-1] < STOP);

	    Time_t ScheduleAt = Manifold :: Now(); // time when we schedule the events.

	    Manifold :: StopAtTime(STOP);
	    Manifold :: Run();

#if 0
	    //Now for verification
	    if(Mytid == 0) {
		//get the received data back for verification.
		unsigned long long recv_data[SIZE];
		MPI_Status status;
		MPI_Recv(recv_data, SIZE, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD, &status);

		//get the received data back for verification.
		Time_t times[SIZE];
		MPI_Recv(times, SIZE, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);

		for(int i=0; i<SIZE; i++) {
		    CPPUNIT_ASSERT_EQUAL(send_data[i], (T)(recv_data[i]));
		    CPPUNIT_ASSERT_DOUBLES_EQUAL(ScheduleAt + when[i] + MyObj1<T>::C0_OUT_LATENCY, times[i], DOUBLE_COMP_DELTA);
		}
	    }
	    else {
		//send the received data back for verification.
		unsigned long long data[SIZE];
		vector<T>& in = c1p->getIncoming();
		for(int i=0; i<SIZE; i++) {
		    data[i] = in[i];
		}
		MPI_Send(data, SIZE, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);

		//send the receive tick back for verification.
		Time_t times[SIZE];
		vector<Time_t>& recv_times = c1p->getRecvTime();
		for(int i=0; i<SIZE; i++) {
		    times[i] = recv_times[i];
		}
		MPI_Send(times, SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	    }
#endif

	    #ifdef DBG
	    std::cout.rdbuf(cout_sbuf);
            #endif
	}




        /**
	 * Build a test suite.
	 */
	static CppUnit::Test* suite()
	{
	    CppUnit::TestSuite* mySuite = new CppUnit::TestSuite("MessagingTest");

	    mySuite->addTest(new CppUnit::TestCaller<MessagingTest>("test_messaging_u32", &MessagingTest::test_messaging<uint32_t>));
	    mySuite->addTest(new CppUnit::TestCaller<MessagingTest>("test_messaging_u64", &MessagingTest::test_messaging<uint64_t>));

	    return mySuite;
	}
};



int main(int argc, char** argv)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    srandom(t.tv_usec);

    Manifold :: Init(argc, argv, Manifold::TIMED, SyncAlg::SA_CMB);


    if(2 != TheMessenger.get_node_size()) {
        cerr << "ERROR: Must specify \"-np 2\" for mpirun!" << endl;
	return 1;
    }

    CppUnit::TextUi::TestRunner runner;
    runner.addTest( MessagingTest::suite() );
    runner.run();

    Manifold :: Finalize();

    return 0;
}

