/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  v10statetests.cpp
 *  @brief: Test v10::CRingStateChangeTiem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingStateChangeItem.cpp"
#include "DataFormat.h"

#include <time.h>
#include <string.h>

class v10statetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10statetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(run_1);
    CPPUNIT_TEST(run_2);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
    void construct_2();
    
    void run_1();
    void run_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10statetest);

// Construction should give a proper state change item.
void v10statetest::construct_1()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item;
    v10::pStateChangeItem p = reinterpret_cast<v10::pStateChangeItem>(item.getItemPointer());
    
    EQ(v10::BEGIN_RUN, p->s_header.s_type);
    EQ(sizeof(v10::StateChangeItem), size_t(p->s_header.s_size));
    EQ(uint32_t(0), p->s_runNumber);
    EQ(uint32_t(0), p->s_timeOffset);
    ASSERT(p->s_Timestamp - now <= 1);
    EQ(0, strcmp(p->s_title, ""));
}
// fully parameterized construction.

void v10statetest::construct_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    v10::pStateChangeItem p = reinterpret_cast<v10::pStateChangeItem>(item.getItemPointer());
    
    EQ(v10::PAUSE_RUN, p->s_header.s_type);
    EQ(sizeof(v10::StateChangeItem), size_t(p->s_header.s_size));
    
    EQ(uint32_t(123), p->s_runNumber);
    EQ(uint32_t(100), p->s_timeOffset);
    EQ(now, time_t(p->s_Timestamp));
    EQ(0, strcmp(p->s_title, "This is a title"));
}
//  Can get run number:

void v10statetest::run_1()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");

    
    EQ(uint32_t(123), item.getRunNumber());
}
void v10statetest::run_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");

    item.setRunNumber(200);
    EQ(uint32_t(200), item.getRunNumber());
}