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

/** @file:  v10counttests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingPhysicsEventCountItem.h"
#include "DataFormat.h"
#include <time.h>
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
class v10counttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10counttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(offset_1);
    CPPUNIT_TEST(offset_2);
    CPPUNIT_TEST(offset_3);
    CPPUNIT_TEST(offset_4);
    
    CPPUNIT_TEST(stamp_1);
    CPPUNIT_TEST(stamp_2);
    
    CPPUNIT_TEST(count_1);
    CPPUNIT_TEST(count_2);
    
    CPPUNIT_TEST(bodyhdr_1);
    
    CPPUNIT_TEST(typstr);
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
    
    void offset_1();
    void offset_2();
    void offset_3();
    void offset_4();
    
    void stamp_1();
    void stamp_2();
    
    void count_1();
    void count_2();
    
    void bodyhdr_1();
    
    void typstr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10counttest);

// Default construction
//   Type is correct,
//   Size is correct and
//   all the other stuff is zero except time stamp which is now.
void v10counttest::construct_1()
{
    time_t now = time(nullptr);
    v10::CRingPhysicsEventCountItem item;             // Default construction.
    
    v10::pPhysicsEventCountItem p =
        reinterpret_cast<v10::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(uint32_t(v10::PHYSICS_EVENT_COUNT), p->s_header.s_type);
    EQ(sizeof(v10::PhysicsEventCountItem), size_t(p->s_header.s_size));
    EQ(uint32_t(0), p->s_timeOffset);
    EQ(uint64_t(0), p->s_eventCount);
    ASSERT((p->s_timestamp - now) <= 1);
}
// Construct at now with count and time offset:

void v10counttest::construct_2()
{
    time_t now = time(nullptr);
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    v10::pPhysicsEventCountItem p =
        reinterpret_cast<v10::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(uint32_t(v10::PHYSICS_EVENT_COUNT), p->s_header.s_type);
    EQ(sizeof(v10::PhysicsEventCountItem), size_t(p->s_header.s_size));
    EQ(uint32_t(10), p->s_timeOffset);
    EQ(uint64_t(100000), p->s_eventCount);
    ASSERT((p->s_timestamp - now) <= 1);
}
// Can get time offset:

void v10counttest::offset_1()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    
    EQ(uint32_t(10), item.getTimeOffset());
}
// Can set time offset:

void v10counttest::offset_2()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    item.setTimeOffset(123);
    EQ(uint32_t(123), item.getTimeOffset());
}
// offset divisor is always 1:

void
v10counttest::offset_3()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    EQ(uint32_t(1), item.getTimeDivisor());
}
// computed time is same as floated offset:

void
v10counttest::offset_4()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    EQ(float(item.getTimeOffset()), item.computeElapsedTime());
}

// Can fetch timestamp:

void v10counttest::stamp_1()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    v10::pPhysicsEventCountItem p =
        reinterpret_cast<v10::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(time_t(p->s_timestamp), item.getTimestamp());
}

// Can set the stamp.
void v10counttest::stamp_2()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    v10::pPhysicsEventCountItem p =
        reinterpret_cast<v10::pPhysicsEventCountItem>(item.getItemPointer());
    item.setTimestamp(1234);
    EQ(time_t(1234), item.getTimestamp());
}

// Can get the count.

void v10counttest::count_1()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    EQ(uint64_t(100000), item.getEventCount());
}
// Can set the count:

void v10counttest::count_2()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    item.setEventCount(200000);
    EQ(uint64_t(200000), item.getEventCount());
}

// Body header pointer is nil.

void v10counttest::bodyhdr_1()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    ASSERT(item.getBodyHeader() == nullptr);
}
// Type string is "Trigger count"

void v10counttest::typstr()
{
    v10::CRingPhysicsEventCountItem item(
        100000, 10
    );
    EQ(std::string("Trigger count"), item.typeName());
}
