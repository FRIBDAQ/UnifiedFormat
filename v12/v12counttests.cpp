/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  v12counttests.cpp
 *  @brief:  Test v12::CRingPhysicsEventCountItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingPhysicsEventCountItem.h"
#include "DataFormat.h"

#include <time.h>

class v12counttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12counttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    
    CPPUNIT_TEST(getOffset_1);
    CPPUNIT_TEST(getOffset_2);
    
    CPPUNIT_TEST(setOffset_1);
    CPPUNIT_TEST(setOffset_2);
    
    CPPUNIT_TEST(elapsed_1);
    CPPUNIT_TEST(elapsed_2);
    
    CPPUNIT_TEST(divisor_1);
    CPPUNIT_TEST(divisor_2);
    
    CPPUNIT_TEST(gettime_1);
    CPPUNIT_TEST(gettime_2);
    
    CPPUNIT_TEST(settime_1);
    CPPUNIT_TEST(settime_2);
    
    CPPUNIT_TEST(getcount_1);
    CPPUNIT_TEST(getcount_2);
    
    CPPUNIT_TEST(originalsid_1);
    CPPUNIT_TEST(originalsid_2);
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
    
    void getOffset_1();
    void getOffset_2();
    
    void setOffset_1();
    void setOffset_2();
    
    void elapsed_1();
    void elapsed_2();
    
    void divisor_1();
    void divisor_2();
    
    void gettime_1();
    void gettime_2();
    
    void settime_1();
    void settime_2();
    
    void getcount_1();
    void getcount_2();
    
    void originalsid_1();
    void originalsid_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12counttest);

// Default constructor ...no body header.  Body zero but for divisor
void v12counttest::construct_1()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item;
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t)
         + sizeof(v12::PhysicsEventCountItemBody),
        size_t(pItem->s_header.s_size)
    );
    
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(uint64_t(0), pBody->s_eventCount);
    
}
/// Constructor with count and offset:

void v12counttest::construct_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10);
    
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t)
         + sizeof(v12::PhysicsEventCountItemBody),     // No body header.
        size_t(pItem->s_header.s_size)
    );
    
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(now, time_t(pBody->s_timestamp));
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(uint64_t(1234), pBody->s_eventCount);
}
// Initial SID

void v12counttest::construct_3()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(12345, 20, now, 2);
    
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader)
         + sizeof(v12::BodyHeader)
         + sizeof(v12::PhysicsEventCountItemBody),    
        size_t(pItem->s_header.s_size)
    );
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(0), pBh->s_barrier);
    
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(20), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(now, time_t(pBody->s_timestamp));
    EQ(uint32_t(2), pBody->s_originalSid);
    EQ(uint64_t(12345), pBody->s_eventCount);
}
// fully specfied construction

void v12counttest::construct_4()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(0x1234567890, 2, 1, 123456, 20, now);
    
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader)
         + sizeof(v12::BodyHeader)
         + sizeof(v12::PhysicsEventCountItemBody),    
        size_t(pItem->s_header.s_size)
    );
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(1), pBh->s_barrier);
    
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(20), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(now, time_t(pBody->s_timestamp));
    EQ(uint32_t(2), pBody->s_originalSid);
    EQ(uint64_t(123456), pBody->s_eventCount);
}
// retrieve time offset with no body header.
void v12counttest::getOffset_1()
{
    CRingPhysicsEventCountItem item(12345, 10);
    EQ(uint32_t(10), item.getTimeOffset());
    
}

// retrieve time offset with body header
void v12counttest::getOffset_2()
{
    CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2);
    
    EQ(uint32_t(10), item.getTimeOffset());
}
// set time offset with no body header.
void v12counttest::setOffset_1()
{
    CRingPhysicsEventCountItem item(12345, 10);
    item.setTimeOffset(20);
    EQ(uint32_t(20), item.getTimeOffset());
}
// set time offset with body header
void v12counttest::setOffset_2()
{
    CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2);
    item.setTimeOffset(35);
    EQ(uint32_t(35), item.getTimeOffset());
}
// compute elapsed with no body header.

void v12counttest::elapsed_1()
{
    CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(float(5), item.computeElapsedTime());
}
// compute elapsed time with body header.
void v12counttest::elapsed_2()
{
    CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2, 2);
    EQ(float(5), item.computeElapsedTime());
}

// get time divisor from no body header.
void v12counttest::divisor_1()
{
    CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint32_t(2), item.getTimeDivisor());
}
// Get time divisor from body header:

void v12counttest::divisor_2()
{
    CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2, 2);
    EQ(uint32_t(2), item.getTimeDivisor());
}
// Get tod stamp from non body header item.

void v12counttest::gettime_1()
{
    time_t now = time(nullptr);
    CRingPhysicsEventCountItem item(12345, 10, 2);
    ASSERT(item.getTimestamp() - now <= 1);    // Due to skew in time.
}
// Get tod stamp from body header item.
void v12counttest::gettime_2()
{
    time_t now = time(nullptr);
    CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(now, item.getTimestamp());
}

void v12counttest::settime_1()
{
    time_t now = time(nullptr);
    CRingPhysicsEventCountItem item(12345, 10, 2);
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
void v12counttest::settime_2()
{
    time_t now = time(nullptr);
    CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    item.setTimestamp(now+20);
    EQ(now+20, item.getTimestamp());
}

// get count from non body header:

void v12counttest::getcount_1()
{
    CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint64_t(12345), item.getEventCount());
}
void v12counttest::getcount_2()
{
    time_t now = time(nullptr);
    CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(uint64_t(1234), item.getEventCount());
}
void v12counttest::originalsid_1()
{
    CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint32_t(0), item.getOriginalSourceId());
}
void v12counttest::originalsid_2()
{
    time_t now = time(nullptr);
    CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(uint32_t(2), item.getOriginalSourceId());
}