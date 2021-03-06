#include <TestFixture.h>
#include <TestAssert.h>
#include <TestSuite.h>
#include <Test.h>
#include <TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <map>
#include <utility>
#include <stdlib.h>

#include "clock.h"

using namespace manifold::kernel;

//####################################################################
// helper classes
//####################################################################
// class MyObj1 is like a component
class MyObj1 {
    private:
	int m_risingTickCalled; //number of time risingTick() has been called
	int m_fallingTickCalled; //number of time fallingTick() has been called
    public:
        MyObj1() : m_risingTickCalled(0), m_fallingTickCalled(0) {}

	int getRisingTickCalled() const { return m_risingTickCalled; }
	int getFallingTickCalled() const { return m_fallingTickCalled; }

	void risingTick()
	{
	    m_risingTickCalled += 1;
	}

	void fallingTick()
	{
	    m_fallingTickCalled += 1;
	}
	void set_clock(Clock&) {} //to make the compiler happy
};




//####################################################################
// Class ClockTest is the unit test class for class Clock. 
//####################################################################
class ClockTest : public CppUnit::TestFixture {
    private:
        static Clock MasterClock;
        static Clock Clock1;
	static const double DOUBLE_COMP_DELTA = 1.0E-5;

    public:
        /**
	 * Initialization function. Inherited from the CPPUnit framework.
	 */
        void setUp()
	{
	}


        /**
	 * Test Rising()
	 */
	void testRising_0()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, MasterClock.getRegistered().size());

            MyObj1* comp1 = new MyObj1();
            MyObj1* comp2 = new MyObj1();

	    //register with master clock
	    tickObjBase* tobj1 = Clock::Register<MyObj1>(comp1, &MyObj1::risingTick, &MyObj1::fallingTick);
	    tickObjBase* tobj2 = Clock::Register<MyObj1>(comp2, &MyObj1::risingTick, &MyObj1::fallingTick);

            int before1 = comp1->getRisingTickCalled();
            int before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            int after1 = comp1->getRisingTickCalled();
            int after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);


            // now disable tobj2
	    tobj2->Disable();

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    //tobj2 disabled, so its risingTick function shouldn't have been called.
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now disable tobj1
	    tobj1->Disable();

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now enable tobj2
	    tobj2->Enable();

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);

            //remove the registered objects for other test cases
            MasterClock.unregisterAll();
	}

        /**
	 * Test Rising()
	 * Test a different clock
	 */
	void testRising_1()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, Clock1.getRegistered().size());

            MyObj1* comp1 = new MyObj1();
            MyObj1* comp2 = new MyObj1();

	    //register with master clock
	    tickObjBase* tobj1 = Clock::Register<MyObj1>(Clock1, comp1, &MyObj1::risingTick, &MyObj1::fallingTick);
	    tickObjBase* tobj2 = Clock::Register<MyObj1>(Clock1, comp2, &MyObj1::risingTick, &MyObj1::fallingTick);

            int before1 = comp1->getRisingTickCalled();
            int before2 = comp2->getRisingTickCalled();

            Clock1.Rising();

            int after1 = comp1->getRisingTickCalled();
            int after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);


            // now disable tobj2
	    tobj2->Disable();

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            Clock1.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    //tobj2 disabled, so its risingTick function shouldn't have been called.
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now disable tobj1
	    tobj1->Disable();

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            Clock1.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now enable tobj2
	    tobj2->Enable();

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            Clock1.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);

            //remove the registered objects for other test cases
            Clock1.unregisterAll();
	}

        /**
	 * Test Falling()
	 */
	void testFalling_0()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, MasterClock.getRegistered().size());

            MyObj1* comp1 = new MyObj1();
            MyObj1* comp2 = new MyObj1();

	    //register with master clock
	    tickObjBase* tobj1 = Clock::Register<MyObj1>(comp1, &MyObj1::risingTick, &MyObj1::fallingTick);
	    tickObjBase* tobj2 = Clock::Register<MyObj1>(comp2, &MyObj1::risingTick, &MyObj1::fallingTick);

            int before1 = comp1->getFallingTickCalled();
            int before2 = comp2->getFallingTickCalled();

            MasterClock.Falling();

            int after1 = comp1->getFallingTickCalled();
            int after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);


            // now disable tobj2
	    tobj2->Disable();

            before1 = comp1->getFallingTickCalled();
            before2 = comp2->getFallingTickCalled();

            MasterClock.Falling();

            after1 = comp1->getFallingTickCalled();
            after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    //tobj2 disabled, so its risingTick function shouldn't have been called.
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now disable tobj1
	    tobj1->Disable();

            before1 = comp1->getFallingTickCalled();
            before2 = comp2->getFallingTickCalled();

            MasterClock.Falling();

            after1 = comp1->getFallingTickCalled();
            after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now enable tobj2
	    tobj2->Enable();

            before1 = comp1->getFallingTickCalled();
            before2 = comp2->getFallingTickCalled();

            MasterClock.Falling();

            after1 = comp1->getFallingTickCalled();
            after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);

            //remove the registered objects for other test cases
            MasterClock.unregisterAll();
	}

        /**
	 * Test Falling()
	 * Test a different clock
	 */
	void testFalling_1()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, Clock1.getRegistered().size());

            MyObj1* comp1 = new MyObj1();
            MyObj1* comp2 = new MyObj1();

	    //register with master clock
	    tickObjBase* tobj1 = Clock::Register<MyObj1>(Clock1, comp1, &MyObj1::risingTick, &MyObj1::fallingTick);
	    tickObjBase* tobj2 = Clock::Register<MyObj1>(Clock1, comp2, &MyObj1::risingTick, &MyObj1::fallingTick);

            int before1 = comp1->getFallingTickCalled();
            int before2 = comp2->getFallingTickCalled();

            Clock1.Falling();

            int after1 = comp1->getFallingTickCalled();
            int after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);


            // now disable tobj2
	    tobj2->Disable();

            before1 = comp1->getFallingTickCalled();
            before2 = comp2->getFallingTickCalled();

            Clock1.Falling();

            after1 = comp1->getFallingTickCalled();
            after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    //tobj2 disabled, so its risingTick function shouldn't have been called.
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now disable tobj1
	    tobj1->Disable();

            before1 = comp1->getFallingTickCalled();
            before2 = comp2->getFallingTickCalled();

            Clock1.Falling();

            after1 = comp1->getFallingTickCalled();
            after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now enable tobj2
	    tobj2->Enable();

            before1 = comp1->getFallingTickCalled();
            before2 = comp2->getFallingTickCalled();

            Clock1.Falling();

            after1 = comp1->getFallingTickCalled();
            after2 = comp2->getFallingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);

            //remove the registered objects for other test cases
            Clock1.unregisterAll();
	}

        /**
	 * Test Insert()
	 * Very implementation dependent.
	 */
	void testInsert_0()
	{
            MyObj1* comp1 = new MyObj1();
	    Ticks_t WHEN = 2;    // WHEN < CLOCK_CALENDAR_LENGTH
	    TickEvent0<MyObj1, MyObj1>* ev0 = new TickEvent0<MyObj1, MyObj1>(WHEN, MasterClock, &MyObj1::risingTick, comp1);
	    MasterClock.Insert(ev0);

            //cast away const because we need to remove the event later.
	    Clock::EventVec_t& v = const_cast<Clock::EventVec_t&>(MasterClock.getCalendar()[WHEN]);

	    std::vector<TickEventBase*>::iterator iter;
	    for(iter=v.begin(); iter != v.end(); ++iter) {
	        if(*iter == ev0) //found
		    break;
	    }
	    CPPUNIT_ASSERT(iter != v.end());

            //remove the event from the calendar
	    v.erase(iter);
	    CPPUNIT_ASSERT_EQUAL(0, (int)v.size());
	}

        /**
	 * Test Insert()
	 * Very implementation dependent.
	 * Schedule an event in far (>= CLOCK_CALENDAR_LENGTH) future
	 */
	void testInsert_1()
	{
	    Ticks_t scheduling = MasterClock.NowTicks();

            MyObj1* comp1 = new MyObj1();
	    Ticks_t WHEN = CLOCK_CALENDAR_LENGTH + 13;
	    TickEvent0<MyObj1, MyObj1>* ev0 = new TickEvent0<MyObj1, MyObj1>(WHEN, MasterClock, &MyObj1::risingTick, comp1);
            Ticks_t idx = (scheduling + ev0->time) * 2;

            //Note Insert() changes ev0->time !!
	    MasterClock.Insert(ev0);

            //cast away const because we need to remove the event later.
            std::multimap<Ticks_t, TickEventBase*>& mmm =  const_cast<std::multimap<Ticks_t, TickEventBase*>&>(MasterClock.getEventMap());

	    std::pair<std::multimap<Ticks_t,TickEventBase*>::iterator, std::multimap<Ticks_t,TickEventBase*>::iterator>
		ret = mmm.equal_range(idx);

	    std::multimap<Ticks_t, TickEventBase*>::iterator it;

	    for(it=ret.first; it != ret.second; ++it) {
	        if((*it).second == ev0) {
		    break;
		}
	    }

	    CPPUNIT_ASSERT(it != ret.second);  // ev0 is found in the multimap

            //remove the event from the map
	    mmm.erase(it);
	    CPPUNIT_ASSERT_EQUAL(0, (int)mmm.size());
	}







        //=========================================================================
        //=========================================================================
	//! @brief Test unregister.
	//!
	//! Create a random number of objects in an array. Register all objects with
	//! the MasterClock. Randomly pick an object A; verify A is in the clock's
	//! list of registered objects. Then call Unregister(); verify A is no longer
	//! in the clock's list.
	void testUnregister_0()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, MasterClock.getRegistered().size());

            //create a random number of objects.
            int Nobjs = random() % 90 + 10; //10 to 99 objects
            MyObj1* comps[Nobjs];

            //register all objects with MasterClock
	    for(int i=0; i<Nobjs; i++) {
	        comps[i] = new MyObj1();
		Clock::Register<MyObj1>(MasterClock, comps[i], &MyObj1::risingTick, &MyObj1::fallingTick);
	    }

	    MyObj1* obj1 = comps[random() % Nobjs]; //randomly pick one.

	    std::list<tickObjBase*>& obj_list = MasterClock.getRegistered();

            //verify obj1 is in MasterClock's registered object list
	    std::list<tickObjBase*>::iterator it = obj_list.begin();
	    for(; it != obj_list.end(); ++it) {
		tickObj<MyObj1>* t = dynamic_cast<tickObj<MyObj1>*>(*it);
		if(t && t->obj == obj1)
		    break;
	    }

            //obj1 must be in the list
	    CPPUNIT_ASSERT(it != obj_list.end());

            //now call Unregister
	    Clock::Unregister<MyObj1>(MasterClock, obj1);

	    obj_list = MasterClock.getRegistered();

            //verify obj1 is removed from MasterClock's registered object list
	    it = obj_list.begin();
	    for(; it != obj_list.end(); ++it) {
		tickObj<MyObj1>* t = dynamic_cast<tickObj<MyObj1>*>(*it);
		if(t && t->obj == obj1)
		    break;
	    }
            //obj1 must NOT be in the list
	    CPPUNIT_ASSERT(it == obj_list.end());

            //remove the registered objects for other test cases
            MasterClock.unregisterAll();
	}





        //=========================================================================
        //=========================================================================
	//! @brief Test unregister.
	//!
	void testUnregister_1()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, MasterClock.getRegistered().size());

            MyObj1* comp1 = new MyObj1();
            MyObj1* comp2 = new MyObj1();

	    //register with master clock
	    tickObjBase* tobj1 = Clock::Register<MyObj1>(MasterClock, comp1, &MyObj1::risingTick, &MyObj1::fallingTick);
	    tickObjBase* tobj2 = Clock::Register<MyObj1>(MasterClock, comp2, &MyObj1::risingTick, &MyObj1::fallingTick);

            int before1 = comp1->getRisingTickCalled();
            int before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            int after1 = comp1->getRisingTickCalled();
            int after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);


            // now unregister comp2
	    Clock::Unregister<MyObj1>(MasterClock, comp2);

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1 + 1, after1);
	    //comp2 was unregistered, so its risingTick function shouldn't have been called.
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now unregister comp1
	    Clock::Unregister<MyObj1>(MasterClock, comp1);

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2, after2);


            // now register comp2 again
	    tobj2->Enable();
	    Clock::Register<MyObj1>(MasterClock, comp2, &MyObj1::risingTick, &MyObj1::fallingTick);

            before1 = comp1->getRisingTickCalled();
            before2 = comp2->getRisingTickCalled();

            MasterClock.Rising();

            after1 = comp1->getRisingTickCalled();
            after2 = comp2->getRisingTickCalled();

	    CPPUNIT_ASSERT_EQUAL(before1, after1);
	    CPPUNIT_ASSERT_EQUAL(before2 + 1, after2);

            //remove the registered objects for other test cases
            MasterClock.unregisterAll();
	}


	void testUnregister_2()
	{
	    //ensure no objects have registered.
	    CPPUNIT_ASSERT_EQUAL((size_t)0, MasterClock.getRegistered().size());

            //create a random number of objects.
            int Nobjs = random() % 90 + 10; //10 to 99 objects
            MyObj1* comps[Nobjs];

            //register all objects with MasterClock
	    for(int i=0; i<Nobjs; i++) {
	        comps[i] = new MyObj1();
		Clock::Register<MyObj1>(MasterClock, comps[i], &MyObj1::risingTick, &MyObj1::fallingTick);
	    }

	    MyObj1* obj1 = comps[random() % Nobjs]; //randomly pick one.




	    std::list<tickObjBase*>& obj_list = MasterClock.getRegistered();

            //verify obj1 is in MasterClock's registered object list
	    std::list<tickObjBase*>::iterator it = obj_list.begin();
	    for(; it != obj_list.end(); ++it) {
		tickObj<MyObj1>* t = dynamic_cast<tickObj<MyObj1>*>(*it);
		if(t && t->obj == obj1)
		    break;
	    }

            //obj1 must be in the list
	    CPPUNIT_ASSERT(it != obj_list.end());





            //now call Unregister
	    Clock& ccc = MasterClock;
	    //Clock::Unregister<MyObj1>(MasterClock, obj1);
	    Clock::Unregister<MyObj1>(ccc, obj1);

	    obj_list = MasterClock.getRegistered();

            //verify obj1 is removed from MasterClock's registered object list
	    it = obj_list.begin();
	    for(; it != obj_list.end(); ++it) {
		tickObj<MyObj1>* t = dynamic_cast<tickObj<MyObj1>*>(*it);
		if(t && t->obj == obj1)
		    break;
	    }
            //obj1 must NOT be in the list
	    CPPUNIT_ASSERT(it == obj_list.end());

            //remove the registered objects for other test cases
            MasterClock.unregisterAll();
	}








        /**
	 * Build a test suite.
	 */
	static CppUnit::Test* suite()
	{
	    CppUnit::TestSuite* mySuite = new CppUnit::TestSuite("ClockTest");

	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testRising_0", &ClockTest::testRising_0));
	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testRising_1", &ClockTest::testRising_1));
	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testFalling_0", &ClockTest::testFalling_0));
	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testFalling_1", &ClockTest::testFalling_1));

	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testInsert_0", &ClockTest::testInsert_0));
	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testInsert_1", &ClockTest::testInsert_1));

	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testUnregister_0", &ClockTest::testUnregister_0));
	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testUnregister_1", &ClockTest::testUnregister_1));
	    mySuite->addTest(new CppUnit::TestCaller<ClockTest>("testUnregister_2", &ClockTest::testUnregister_2));
	    return mySuite;
	}
};

Clock ClockTest::MasterClock = Clock(10);
Clock ClockTest::Clock1 = Clock(6);


int main()
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( ClockTest::suite() );
    if(runner.run("", false))
	return 0; //all is well
    else
	return 1;
}



