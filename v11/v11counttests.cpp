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

/** @file:  v11counttests.cpp
 *  @brief: Tests for  v11::CRingPhysicsEventCountItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingPhysicsEventCountItem.h"
#include "DataFormat.h"
#include <time.h>

class v11counttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11counttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    
    CPPUNIT_TEST(offset_1);
    CPPUNIT_TEST(offset_2);
    CPPUNIT_TEST(offset_3);
    CPPUNIT_TEST(offset_4);
    
    CPPUNIT_TEST(abstime_1);
    CPPUNIT_TEST(abstime_2);
    
    CPPUNIT_TEST(evtcount_1);
    CPPUNIT_TEST(evtcount_2);
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
    void construct_3();
    void construct_4();
    
    void offset_1();
    void offset_2();
    void offset_3();
    void offset_4();
    
    void abstime_1();
    void abstime_2();
    
    void evtcount_1();
    void evtcount_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11counttest);

// Default constructor.
void v11counttest::construct_1()
{
    time_t now = time(nullptr);
    v11::CRingPhysicsEventCountItem item;
    v11::pPhysicsEventCountItem pItem =
        reinterpret_cast<v11::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t) + sizeof(v11::PhysicsEventCountItemBody),
       size_t(pItem->s_header.s_size));
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    v11::pPhysicsEventCountItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint64_t(0), pBody->s_eventCount);
}
// construtcor with count and run relative timng.

void v11counttest::construct_2()
{
    time_t now = time(nullptr);
    
    v11::CRingPhysicsEventCountItem item(12345, uint32_t(10), 2);
    
    v11::pPhysicsEventCountItem pItem =
        reinterpret_cast<v11::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t) + sizeof(v11::PhysicsEventCountItemBody),
       size_t(pItem->s_header.s_size));
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    v11::pPhysicsEventCountItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(2), pBody->s_offsetDivisor);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint64_t(12345), pBody->s_eventCount);
}
// Construct with absolute time.
void v11counttest::construct_3()
{
    time_t now = time(nullptr);
    now -= 20;
    
    v11::CRingPhysicsEventCountItem item(12345, 10, now, 2);
    v11::pPhysicsEventCountItem pItem =
        reinterpret_cast<v11::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t) + sizeof(v11::PhysicsEventCountItemBody),
       size_t(pItem->s_header.s_size));
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    v11::pPhysicsEventCountItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(2), pBody->s_offsetDivisor);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint64_t(12345), pBody->s_eventCount);
}
// fully specified construction with body header:

void v11counttest::construct_4()
{
    time_t now = time(nullptr);
    now -= 20;
    
    v11::CRingPhysicsEventCountItem item(12345, 10, now, 0x1234567890, 2);
    v11::pPhysicsEventCountItem pItem =
        reinterpret_cast<v11::pPhysicsEventCountItem>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader) + sizeof(v11::PhysicsEventCountItemBody),
       size_t(pItem->s_header.s_size));
    
    v11::pBodyHeader pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(0), pBh->s_barrier);
    
    v11::pPhysicsEventCountItemBody pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint64_t(12345), pBody->s_eventCount);
    
}
// Can get time offset:

void v11counttest::offset_1()
{
    v11::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint32_t(10), item.getTimeOffset());
}
// can set time offset:

void v11counttest::offset_2()
{
    v11::CRingPhysicsEventCountItem item(12345, 10, 2);
    item.setTimeOffset(25);
    EQ(uint32_t(25), item.getTimeOffset());
}
// can get the time divisor

void v11counttest::offset_3()
{
    v11::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint32_t(2), item.getTimeDivisor());
}
// Get compute elapsed time taking into account the divisor:

void v11counttest::offset_4()
{
    v11::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(float(5.0), item.computeElapsedTime());
}

// Can get the absolute timestamp:

void v11counttest::abstime_1()
{
    time_t now = time(nullptr);
    now -= 20;
    
    v11::CRingPhysicsEventCountItem item(12345, 10, now, 2);
    EQ(now, item.getTimestamp());
    
}
// can set absolute timestamp:

void v11counttest::abstime_2()
{
    time_t now = time(nullptr);
    now -= 20;
    
    v11::CRingPhysicsEventCountItem item(12345, 10, now, 2);
    item.setTimestamp(now+20);
    EQ(now+20, item.getTimestamp());
}
// can get the event count:

void v11counttest::evtcount_1()
{
    v11::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint64_t(12345), item.getEventCount());
}
// can set the event count.

void v11counttest::evtcount_2()
{
    v11::CRingPhysicsEventCountItem item(12345, 10, 2);
    item.setEventCount(42);
    EQ(uint64_t(42), item.getEventCount());
}