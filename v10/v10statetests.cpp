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
using namespace ufmt;
class v10statetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10statetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(run_1);
    CPPUNIT_TEST(run_2);
    
    CPPUNIT_TEST(elapsed_1);
    CPPUNIT_TEST(elapsed_2);
    
    CPPUNIT_TEST(divisor);
    CPPUNIT_TEST(computeelapsed);
    
    CPPUNIT_TEST(title_1);
    CPPUNIT_TEST(title_2);
    
    CPPUNIT_TEST(tstamp_1);
    CPPUNIT_TEST(tstamp_2);
    
    CPPUNIT_TEST(bodyhdr);
    
    CPPUNIT_TEST(type_1);
    CPPUNIT_TEST(type_2);
    CPPUNIT_TEST(type_3);
    CPPUNIT_TEST(type_4);
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
    
    void elapsed_1();
    void elapsed_2();
    
    void divisor();
    void computeelapsed();
    
    void title_1();
    void title_2();
    
    void tstamp_1();
    void tstamp_2();
    
    void bodyhdr();
    
    void type_1();
    void type_2();
    void type_3();
    void type_4();
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
// See if we can set run number:

void v10statetest::run_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");

    item.setRunNumber(200);
    EQ(uint32_t(200), item.getRunNumber());
}
// See if we can fetch elapsed time

void v10statetest::elapsed_1()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    EQ(uint32_t(100), item.getElapsedTime());
}
// See if we can set the elapsed time:

void v10statetest::elapsed_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    item.setElapsedTime(200);
    EQ(uint32_t(200), item.getElapsedTime());
}
// divisor is hard coded to 1:

void v10statetest::divisor()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    EQ(uint32_t(1), item.getTimeDivisor());
}
// Elapsed time is the same as the raw time:

void v10statetest::computeelapsed()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    EQ(float(100), item.computeElapsedTime());
}
// Can fetch title:
void v10statetest::title_1()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    EQ(std::string("This is a title"), item.getTitle());
    
}
// Can set title.
void v10statetest::title_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    item.setTitle("Changed the title");
    EQ(std::string("Changed the title"), item.getTitle());
}

// Can get time_t
void v10statetest::tstamp_1()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    EQ(now, item.getTimestamp());
}
// Can set time_t
void v10statetest::tstamp_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    item.setTimestamp(now+100);
    EQ(now+100, item.getTimestamp());
}
// body header pointer is always null.
void v10statetest::bodyhdr()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    ASSERT(item.getBodyHeader() == nullptr);
}
// Type name tests.

void v10statetest::type_1()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::PAUSE_RUN, 123, 100, now, "This is a title");
    
    EQ(std::string(" Pause Run "), item.typeName());
}
void v10statetest::type_2()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::BEGIN_RUN, 123, 100, now, "This is a title");
    
    EQ(std::string(" Begin Run "), item.typeName());
}
void v10statetest::type_3()
{
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::RESUME_RUN, 123, 100, now, "This is a title");
    
    EQ(std::string(" Resume Run "), item.typeName());
}
void v10statetest::type_4() {
    time_t now = time(nullptr);
    v10::CRingStateChangeItem item(v10::END_RUN, 123, 100, now, "This is a title");
    
    EQ(std::string(" End Run "), item.typeName());
    
}